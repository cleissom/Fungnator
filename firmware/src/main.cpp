#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FS.h>

const char* SSID = "RedePI3"; //Seu SSID da Rede WIFI
const char* PASSWORD = "noisqtapi3"; // A Senha da Rede WIFI

const char* MQTT_SERVER = "test.mosquitto.org";
int         PORT = 1883;

/*
const char* USER = "rtsukiae";
const char* USER_PASSWORD = "tJ57VNAJ_U9t";
const char* MQTT_SERVER = "m10.cloudmqtt.com";
int         PORT = 18650;
*/

#define GPIO_PIN_HEATER     14
#define GPIO_PIN_HUMIDIFIER 12
#define GPIO_PIN_FAN1       15  // Circulação de Ar
#define GPIO_PIN_FAN2       13  // Entrada de Ar

#define DHTPIN 2
#define DHTTYPE DHT22
#define REPORT_INTERVAL 5 // in sec

#define TEMPERATURE_SMOOTHING_CONSTANT  0.4
#define HUMIDITY_SMOOTHING_CONSTANT     0.8

DHT           dht(DHTPIN, DHTTYPE);
WiFiClient    CLIENT;
WiFiUDP       ntpUDP;
PubSubClient  MQTT(CLIENT);
NTPClient     timeClient(ntpUDP);
char      espID[6];
long      lastMsg = 0;
char      msg[50];
float     temperature = 0;
float     humidity = 0;
uint32_t  humidifierEpoch = 0;

struct systemData {
    uint8_t   state;
    float     setTemperature;
    float     setHumidity;
    uint32_t  startEpochTime;
    uint32_t  humidifierPeriod;       // in minutes
    uint32_t  humidifierActiveTime;   // in seconds
} systemData;

void processSensors();
void processActuators();
void processHeater();
void processHumidifier();
void processFan1();
void processFan2();
void setupWIFI();
void reconectar();
void setupPin();
void setupSubscriptions();
void sendStatus();
void loadSystemData();
void saveSystemData();
void callback(char* topic, byte* payload, unsigned int length);


void setup(void) {
  Serial.begin(115200);

  uint8_t temp[6];
  setupWIFI();
  WiFi.macAddress(temp);
  sprintf(espID, "%X%X%X", temp[3], temp[4], temp[5]);
  Serial.println(espID);

  MQTT.setServer(MQTT_SERVER, PORT);
  MQTT.setCallback(callback);

  setupPin();

  dht.begin();

  loadSystemData();

  timeClient.begin();
}

void loop(void) {

  if (WiFi.status() != WL_CONNECTED) {
    setupWIFI();
  }

  if (!MQTT.connected()) {
    reconectar();
  }
  MQTT.loop();

  if (systemData.state == 1)
  {
    processSensors();
    processActuators();
    sendStatus();
  }

  delay(1000 * REPORT_INTERVAL);

}

void processSensors(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

if(!isnan(t))
  temperature = temperature + TEMPERATURE_SMOOTHING_CONSTANT * (t - temperature);

if(!isnan(h))
  humidity    = humidity    + TEMPERATURE_SMOOTHING_CONSTANT * (h - humidity);
}

void processActuators(){
  processHeater();
  processHumidifier();
  processFan1();
  processFan2();
}

void processHeater(){
  float error = systemData.setTemperature - temperature;
  if(error > 0)
    digitalWrite(GPIO_PIN_HEATER, HIGH);
  else
    digitalWrite(GPIO_PIN_HEATER, LOW);
}

void processHumidifier(){
  uint32_t actualEpoch = timeClient.getEpochTime();

  if((actualEpoch >= humidifierEpoch) && (systemData.humidifierActiveTime != 0) && (systemData.humidifierPeriod != 0)){
    Serial.println("periodic humidifier");
    if(!digitalRead(GPIO_PIN_HUMIDIFIER))
      digitalWrite(GPIO_PIN_HUMIDIFIER, HIGH);
    if(actualEpoch > (humidifierEpoch + systemData.humidifierActiveTime)){
      Serial.println("stopped periodic humidifier");
      humidifierEpoch = actualEpoch + (systemData.humidifierPeriod * 60);
      digitalWrite(GPIO_PIN_HUMIDIFIER, LOW);
    }
  } else {
    Serial.println("error humidifier");
    float error = systemData.setHumidity - humidity;
    if(error > 0)
      digitalWrite(GPIO_PIN_HUMIDIFIER, HIGH);
    else
      digitalWrite(GPIO_PIN_HUMIDIFIER, LOW);
  }

}

void processFan1(){
  if(!digitalRead(GPIO_PIN_FAN1))
    digitalWrite(GPIO_PIN_FAN1, HIGH);
}

void processFan2(){
  if(!digitalRead(GPIO_PIN_FAN2))
    digitalWrite(GPIO_PIN_FAN2, HIGH);
}

//CONFIGURAÇÃO DA INTERFACE DE REDE
void setupWIFI() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando na rede: ");
  Serial.println(SSID);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  }
}

void reconectar() {
  if (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT.");
    if (MQTT.connect(espID)) {
      Serial.println("Conectado com Sucesso ao Broker");

        setupSubscriptions();

    } else {
      Serial.print("Falha ao Conectador, rc=");
      Serial.print(MQTT.state());
    }
  }
}

void setupPin() {
  pinMode(GPIO_PIN_HEATER, OUTPUT);
  digitalWrite(GPIO_PIN_HEATER, LOW);

  pinMode(GPIO_PIN_HUMIDIFIER, OUTPUT);
  digitalWrite(GPIO_PIN_HUMIDIFIER, LOW);

  pinMode(GPIO_PIN_FAN1, OUTPUT);
  digitalWrite(GPIO_PIN_FAN1, LOW);

  pinMode(GPIO_PIN_FAN2, OUTPUT);
  digitalWrite(GPIO_PIN_FAN2, LOW);
}

void setupSubscriptions(){
  char topic_buff[20];
  sprintf(topic_buff, "%s/set/#", espID);
  MQTT.subscribe(topic_buff, 1);
  Serial.print("Subscribed to ");
  Serial.println(topic_buff);
}

void sendStatus(){
  char topic[20];
  sprintf(msg, "%.1f", temperature);
  sprintf(topic, "%s/status/temperature", espID);
  MQTT.publish(topic, msg, true);

  sprintf(msg, "%.1f", humidity);
  sprintf(topic, "%s/status/humidity", espID);
  MQTT.publish(topic, msg, true);

  Serial.print("H: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("T: ");
  Serial.println(temperature);

  timeClient.update();
  uint32_t daysElapsed = (timeClient.getEpochTime() - systemData.startEpochTime)/86400;
  sprintf(msg, "%d", daysElapsed);
  sprintf(topic, "%s/status/elapsed", espID);
  MQTT.publish(topic, msg, true);

////////////////////
  sprintf(msg, "%d", systemData.startEpochTime);
  sprintf(topic, "%s/status/elapsed2", espID);
  MQTT.publish(topic, msg, true);
////////////////////

  sprintf(msg, "%d", digitalRead(GPIO_PIN_HEATER));
  sprintf(topic, "%s/status/heater", espID);
  MQTT.publish(topic, msg, true);

  sprintf(msg, "%d", digitalRead(GPIO_PIN_HUMIDIFIER));
  sprintf(topic, "%s/status/humidifier", espID);
  MQTT.publish(topic, msg, true);

  sprintf(msg, "%d", digitalRead(GPIO_PIN_FAN1));
  sprintf(topic, "%s/status/fan1", espID);
  MQTT.publish(topic, msg, true);

  sprintf(msg, "%d", digitalRead(GPIO_PIN_FAN2));
  sprintf(topic, "%s/status/fan2", espID);
  MQTT.publish(topic, msg, true);
}

void loadSystemData(){

  if(SPIFFS.begin()){
    Serial.println("SPIFFS Initialize....ok");
  } else {
    Serial.println("SPIFFS Initialization...failed");
  }

  File f = SPIFFS.open("systemData.txt", "r");

  if (!f) {
    Serial.println("file open failed");
    systemData.state = 0;
  } else {
      Serial.println("Reading Data from File:");
      //Data from file
      f.read((uint8_t*)&systemData, sizeof(systemData));

      f.close();  //Close file
      Serial.println("File Closed");
  }
}

void saveSystemData(){

  if(SPIFFS.begin()){
    Serial.println("SPIFFS Initialize....ok");
  } else {
    Serial.println("SPIFFS Initialization...failed");
  }

  File f = SPIFFS.open("systemData.txt", "w");

  if (!f) {
    Serial.println("file open failed");

  } else {
      Serial.println("Writing Data from File:");
      //Data from file
      f.write((uint8_t*)&systemData, sizeof(systemData));

      f.close();  //Close file
      Serial.println("File Closed");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  String msg_buff;
  for (int i = 0; i < length; i++)
    msg_buff += (char)payload[i];

  Serial.println("Received:");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("payload: ");
  Serial.println(msg_buff);

  char topic_buff[50];

  sprintf(topic_buff, "%s/set/state", espID);
  if (strcmp(topic, topic_buff) == 0)
  {
    if (msg_buff == String('1'))
    {
      if(systemData.state != 1)
      {
        systemData.state = 1;
        timeClient.update();
        systemData.startEpochTime = timeClient.getEpochTime();
        saveSystemData();
      }
    }
    else
    {
      if(systemData.state != 0)
      {
        systemData.state = 0;
        saveSystemData();
        digitalWrite(GPIO_PIN_HEATER, LOW);
        digitalWrite(GPIO_PIN_HUMIDIFIER, LOW);
        digitalWrite(GPIO_PIN_FAN1, LOW);
        digitalWrite(GPIO_PIN_FAN2, LOW);
        sendStatus();
      }
    }
  }

  sprintf(topic_buff, "%s/set/temperature", espID);
  if (strcmp(topic, topic_buff) == 0)
  {
      systemData.setTemperature = msg_buff.toFloat();
      saveSystemData();
  }

  sprintf(topic_buff, "%s/set/humidity", espID);
  if (strcmp(topic, topic_buff) == 0)
  {
      systemData.setHumidity = msg_buff.toFloat();
      saveSystemData();
  }

  sprintf(topic_buff, "%s/set/humidifier/period", espID);
  if (strcmp(topic, topic_buff) == 0)
  {
      systemData.humidifierPeriod = msg_buff.toInt();
      saveSystemData();
  }

  sprintf(topic_buff, "%s/set/humidifier/activetime", espID);
  if (strcmp(topic, topic_buff) == 0)
  {
      systemData.humidifierActiveTime = msg_buff.toInt();
      saveSystemData();
  }

}
