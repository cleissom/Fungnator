/*
 * system.c
 *
 * Created: 11/29/2017 5:30:33 PM
 *  Author: Cleissom
 */

/* Header for all project specific definitions.
F_CPU, Times, Thresholds, Ports and others */
#include "definitions.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "system.h"

#include "menu.h"
#include "rtc.h"

/* Global variable that is set when cycle is started */
uint8_t cycleStarted;

extern rtcData_t   rtcData;
extern menuFlags_t menuFlags;

/* External variable that holds hour from last rtcUpdate */
extern uint8_t lastHour;

/* Cycle data that is saved on EEPROM Memory */
initData_t EEMEM initData;

void checkSystemInitStatus(void)
{
	if(eeprom_read_byte(&initData.cycleStarted) != 0) {
		cycleStarted = 1;
	}
}

void startCycle(void)
{
	cycleStarted = 1;
	initData_t tempData;
	tempData.cycleStarted = 1;
	tempData.day = rtcData.day;
	tempData.month = rtcData.month;
	tempData.year = rtcData.year;
	tempData.daysElapsed = 0;
	eeprom_update_block(&tempData, &initData, 5);
}

void stopCycle(void)
{
	cycleStarted = 0;
	initData_t tempData;
	tempData.cycleStarted = 0;
	tempData.day = 0;
	tempData.month = 0;
	tempData.year = 0;
	tempData.daysElapsed = 0;
	eeprom_update_block(&tempData, &initData, 5);
}

void updateDaysElapsed(void)

{
	if((lastHour == 23) && (rtcData.hours == 0)) {
		eeprom_update_byte(&initData.daysElapsed, (eeprom_read_byte(&initData.daysElapsed) + 1));
	}
}