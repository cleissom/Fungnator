

# Index
- [Motivação](#motivação)
- [Introdução](#introdução)
	- [O Cultivo de Cogumelos](#o-cultivo-de-cogumelos)
	- [O Protocolo MQTT](#o-protocolo-mqtt)
 - [Sensoreamento](#sensoreamento)
	 - [Componentes](#componentes)
	 - [Diagrama de Blocos](#diagrama-de-blocos)
	 - [Placa de Circuito Impresso](#placa-de-circuito-impresso)
 - [Testes](#testes)
 - [Softwares](#softwares)
	 -  [Servidor MQTT](#servidor-mqtt)
	 - [Firmware do ESP8266](#firmware-do-esp8266)
- [Orçamentos](#orçamentos)
- [Montagem](#montagem)
- 

# Motivação

O intuito  é projetar e desenvolver uma estufa para cultuvo de cogumelos de modo descentralizado e utilizando o protocolo MQTT para comunicação via Wi-Fi.
Inicialmente será feito um protótipo de um modelo menor e será deixado como uma base para se poder expandir futuramente.

# Introdução

## O Cultivo de Cogumelos
Hoje em dia está tendo uma demanda crescente para o consumo de cogumelos, seja ela devido ao seu delicioso sabor, ao uso nos pratos de origem Orientais, ou ao veganismo, na qual não se consomem produtos de origem animal.
O cultivo de cogumelo é tido como um taboo para as pessoas. Isso pode ser devido à diferença dos cogumelos com os alimentos que encontramos no dia a dia. Um cogumelo não é uma planta e nem um animal, ele é algo próprio, um fungo. Um fungo quase sempre é levado para uma conotação ruim, como o mofo ou bolor. Porém, alguns desses fungos são saborosos e nutritivos e infelizmente são pouco comercializados.



### Estágios de Cultivo

### Cogumelos Mais Comuns
- Cogumelo Paris ou Champignon:
<img src="https://cdn.awsli.com.br/1000x1000/334/334766/produto/18044212/17f8c200a1.jpg" width="300" />
- Shimeji: 
<img src="https://http2.mlstatic.com/cogumelo-shimeji-branco-fresco-r-3000-o-kilo-D_NQ_NP_835450-MLB26598211450_012018-F.webp" width="300" />
- Shitake:
<img src="http://www.casacamponesa.com.br/sites/default/files/produtos/cogumelo-shitake.jpg" width="300" />
- Cogumelo Salmão ou Shimeji Rosa:
<img src="http://www.casacamponesa.com.br/sites/default/files/produtos/cogumelo-salmao.jpg" width="300" />
- Portobello:
<img src="https://static.wixstatic.com/media/9a994c_a21487f1a5af481b80e415b6e3e3525d~mv2.jpg/v1/fill/w_494,h_299,al_c,q_90/9a994c_a21487f1a5af481b80e415b6e3e3525d~mv2.webp" width="300" />

## O Protocolo MQTT
**MQTT**, acrônimo de Message Queuing Telemetry Transport (anteriormente conhecido como _MQ Telemetry Transport_), é um protocolo de mensagens leve para sensores e pequenos dispositivos móveis otimizado para redes TCP/IP não confiáveis ou de alta latência. O esquema de troca de mensagens é fundamentado no modelo de publicação e assinatura.

O MQTT é um protocolo de rede leve e flexível que oferece o equilíbrio ideal para os desenvolvedores de IoT. O protocolo leve permite a implementação em hardware de dispositivo altamente restringido e em redes de largura da banda limitada e de alta latência e sua flexibilidade possibilita o suporte a diversos cenários de aplicativo para dispositivos e serviços de IoT.

O protocolo MQTT define dois tipos de entidades na rede: um message broker e inúmeros clientes. O broker é um servidor que recebe todas as mensagens dos clientes e, em seguida, roteia essas mensagens para os clientes de destino relevantes. Um cliente é qualquer coisa que possa interagir com o broker e receber mensagens. Um cliente pode ser um sensor de IoT em campo ou um aplicativo em um data center que processa dados de IoT.

1.  O cliente conecta-se ao broker. Ele pode assinar qualquer "tópico" de mensagem no broker. Essa conexão pode ser uma conexão TCP/IP simples ou uma conexão TLS criptografada para mensagens sensíveis.
2.  O cliente publica as mensagens em um tópico, enviando a mensagem e o tópico ao broker.
3.  Em seguida, o broker encaminha a mensagem a todos os clientes que assinam esse tópico. 

![mqtt1](https://www.ibm.com/developerworks/library/iot-mqtt-why-good-for-iot/image1.png)
[[Source]](https://www.ibm.com/developerworks/br/library/iot-mqtt-why-good-for-iot/)

# Sensoreamento
## Componentes
- **Micrcontrolador**: ESP8266
- **Sensor de temperatura**: DHT22 
- **Sensor de umidade**: DHT22 
- **Ventilação**: Mini Ventilador
- **Aquecimento**: Aquecedor de Aquário
- **Umidificação**: Umidificador Ultrassônico "Fogger"

### ESP8266
O ESP8266 é um microcontrolador de baixo custo da fabricante chinêsa Espressif que inclui capacidade de comunicação por Wi-Fi. É um microcontrolador de 32 bits e que possui uma grande variedade de módulos. O módulo empregado para esse projeto foi o Kit de Desenvolvimento Node-MCU.

<img src="https://cdn.shopify.com/s/files/1/0672/9409/products/NodeMCU_ESP8266_development_board_1024x1024.jpg" width="200" />

### DHT22
O DHT22 é um sensor de temperatura e umidade que usa comunicação One-Wire. Este sensor foi escolhido para o projeto por ser o sensor mais disponível no mercado brasileiro.

<img src="https://uploads.filipeflop.com/2017/07/SKU031549-.2-600x600.jpg" width="200" />


### Mini Ventilador
Mini Ventiladores são o suficiente para promover o fluxo de ar interno e para promover a troca de ar.

<img src="https://cdn.shopify.com/s/files/1/0447/3693/products/scythe-mini-kaze-ultra-40-x-20-mm-silent-mini-fan-sy124020l-4_800x.jpeg" width="200" />

### Aquecedor de Aquário
Usar um aquecedor de aquário dentro de um recipiente com água é um método simples, prático, barato e replicável para se aquecer um ambiente.

<img src="https://http2.mlstatic.com/aquecedor-para-aquarios-sarlo-better-hot-25w-220v-D_NQ_NP_994011-MLB20453932090_102015-F.jpg" width="200" />

### Umidificador Ultrassônico "Fogger"
O Fogger é mergulhado na água e então ele cria uma névoa em sua superfície por meio de ondas ultrassônicas. Essa névoa pode ser utilizada para umidificar o ar.

<img src="https://img1.wantitall.co.za/prodimages/new-ultrasonic-fogger-mist-maker-fog-water-fountain-pond-atomizer-air-humidifier__41G-NPwk1pL.jpg" width="200" />


## Diagrama de Blocos
![Diagrama de Blocos](https://raw.githubusercontent.com/Everton-LF-Santos/Projeto-Integrador-3-2018-1/Estufa-para-Cogumelos/imgs/block_diagram.jpg)

A parte do sensoreamento é formada pelo ESP8266 como unidade central e este recebe os sinais dos sensores e manda sinal para os atuadores.

## Placa de Circuito Impresso
A placa de circuito impresso foi feita no software Altium.
### Esquemático
![Diagrama de Blocos](https://raw.githubusercontent.com/Everton-LF-Santos/Projeto-Integrador-3-2018-1/Estufa-para-Cogumelos/imgs/schematic.jpg)


### PCB
![Diagrama de Blocos](https://raw.githubusercontent.com/Everton-LF-Santos/Projeto-Integrador-3-2018-1/Estufa-para-Cogumelos/imgs/pcb.jpg)

# Testes
## Caixa de isopor
Foi feito um teste de resfriamento da caixa de isopor e numericamente encontrado a curva resultante do comportamento dos dados:
![isopor](https://raw.githubusercontent.com/Everton-LF-Santos/Projeto-Integrador-3-2018-1/Estufa-para-Cogumelos/imgs/temperature_curve_box_cooling.png)
Podemos aproximar a função da temperatura pelo tempo por T(t)=28.3+18.15\*exp(-0.001225\*t).
Com isso podemos obter a condutividade térmica da caixa de isopor, k = 0.001225.



# Softwares

## Servidor MQTT
Foi utilizado o servidor test.mosquitto.org como servidor MQTT. Este servidor é de livre uso utilizando a porta 1883.

## Firmware do ESP8266

O Firmware do ESP8266 foi feito utilizando a framework do Arduino.

# Orçamentos
A tabela de preços contem valores aproximados:

|Item						|Preço	|
|---------------------------|------:|
|ESP8266					| 25 R$	|
|DHT22						| 35 R$	|
|Mini Ventilador			| 6 R$	|
|Aquecedor de Aquário 50W	| 30 R$	|
|Fogger						| 20 R$	|
|Caixa de Isopor 30L		| 30 R$	|
|**Total**					| 146 R$	|


# Montagem

# Manual de Uso
O ESP8266 se increverá no tópico "/set/" do seu ID. Cada ESP8266 tem um ID, que será designado ESPID. 

Para iniciar, deve-se mandar "1" para o tópico:
```
ESPID/set/state
```
Para ajustar a temperatura, deve-se mandar o valor desejado para:
```
ESPID/set/temperature
```
Para ajustar a umidade, deve-se mandar o valor desejado para:
```
ESPID/set/humidity
```
Para ajustar o tempo em que o Umidificador permanece ativo temporariamente, deve-se mandar o valor desejado, em segundos, para:
```
ESPID/set/humifer/activetime
```
Para ajustar o periodo em que o Umidificador será ligado, deve-se mandar o valor desejado, em minutos, para:
```
ESPID/set/humifer/period
```
\
\
O ESP8266 mandará informações nos seguintes tópicos:
```
ESPID/status/temperature
ESPID/status/humidity
ESPID/status/elapsed
ESPID/status/heater
ESPID/status/humidifier
ESPID/status/fan1
ESPID/status/fan2
```