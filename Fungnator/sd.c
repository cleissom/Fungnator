/*
 * sd.c
 *
 * Created: 11/30/2017 11:54:10 PM
 *  Author: Cleissom
 */

/* Header for all project specific definitions.
F_CPU, Times, Thresholds, Ports and others*/
#include "definitions.h"

#include <avr/io.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include "sd.h"

#include "actuators.h"
#include "ff.h"
#include "rtc.h"
#include "system.h"

#ifdef DEBUG
#include "uart.h"
#endif /* DEBUG */


extern rtcData_t rtcData;
extern actuatorStatus_t actuators;
extern initData_t initData;
extern uint8_t cycleStarted;
extern uint8_t lastHour;

/* Variable that keeps start hour to write on SDCard */
uint8_t EEMEM writeTime_S;
uint8_t	writeTime;

FATFS sdCard;
FIL file;
FRESULT res;

/*
	@brief	Starts a new file whose name is the date of initialization and then writes
			"definitions.h" configurations on header of file.
*/
void initSd(void)
{
	res = f_mount(0, &sdCard);

#ifdef DEBUG
	if(res == FR_OK) {
		uart_puts("Cartao montado\r");
	}
#endif /* DEBUG */


	uint8_t buffer[11];
	sprintf(buffer, "%02d%02d%02d.dat", rtcData.year, rtcData.month, rtcData.day);

	res = f_open(&file, buffer, FA_WRITE | FA_OPEN_ALWAYS);

#ifdef DEBUG
	if(res == FR_OK) {
		uart_puts("Novo arquivo aberto\r");
	}
	if(res == FR_DISK_ERR) {
		uart_puts("DISK_ERR\r");
	}
	if(res == FR_INT_ERR) {
		uart_puts("INT_ERR\r");
	}
	if(res == FR_NOT_READY) {
		uart_puts("NOT_READY\r");
	}
	if(res == FR_DENIED) {
		uart_puts("DENIED\r");
	}
	if(res == FR_NOT_ENABLED) {
		uart_puts("NOT_ENABLED\r");
	}
	if(res == FR_NO_FILESYSTEM) {
		uart_puts("NO_FILESYSTEM\r");
	}
	if(res == FR_TIMEOUT) {
		uart_puts("TIMEOUT\r");
	}
#endif /* DEBUG */

	f_lseek(&file, f_size(&file));
	f_printf(&file, "TEMPERATURE_MAX:%d;TEMPERATUER_MIN:%d;\n"
	         "HUMIDITY_MAX:%d;HUMIDITY_MIN:%d;\n"
	         "LIGHT_START_HOUR:%d;LIGHT_STOP_HOUR:%d\n"
	         "FRESH_AIR_FAN_ACTIVE_TIME:%d;FRESH_AIR_FAN_PERIOD:%d\n"
	         "YEAR/MONTH/DAY;LIGHTENABLED\n"
	         , TEMPERATURE_MAX, TEMPERATURE_MIN,
	         HUMIDITY_MAX, HUMIDITY_MIN,
	         LIGHT_START_HOUR, LIGHT_STOP_HOUR,
	         FRESH_AIR_FAN_ACTIVE_TIME, FRESH_AIR_FAN_PERIOD);
	f_sync(&file);
	writeTime = rtcData.hours;

	eeprom_write_byte(&writeTime_S, writeTime);
}

void terminateSd(void)
{
	f_close(&file);
}

/*
	@brief	Check if Cycle is started on CycleStarted variable of "system.c" then, if this is true,
			load file whose name is the date of initialization stored on eeprom
*/
void checkSd(void)
{
	if(cycleStarted) {
		res = f_mount(0, &sdCard);
		if(res == FR_OK) {
			uart_puts("Cartao montado\r");
		}
		uint8_t buffer[10];
		sprintf(buffer, "%02d%02d%02d.dat", eeprom_read_byte(&initData.year), eeprom_read_byte(&initData.month), eeprom_read_byte(&initData.day));
		res = f_open(&file, buffer, FA_WRITE | FA_OPEN_ALWAYS);
		if(res == FR_OK) {
			uart_puts("Antigo arquivo aberto\r");
		}
		f_lseek(&file, f_size(&file));
		writeTime = eeprom_read_byte(&writeTime_S);
	}
}

/*
	@brief	Writes on SdCard when there is a transition on hour on writeTime and next hour;
*/
void writeDataSd(void)
{
	if((lastHour == writeTime) && (rtcData.hours == (writeTime + 1))) {
		f_printf(&file, "%02d/%02d/%02d;%d\r\n", rtcData.year, rtcData.month, rtcData.day, actuators.lightEnabled);
		f_sync(&file);
	}
}