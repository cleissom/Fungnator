/*
 * rtc.c
 *
 * Created: 11/28/2017 11:43:50 PM
 *  Author: Cleissom
 */

#include <avr/io.h>
#include "rtc.h"
#include "twimaster.h"
#include "ds1307.h"
#include "uart.h"

/* Global variable that holds rtc data */
rtcData_t rtcData;

/* Global variable that holds hour from last rtcUpdate.
Used for counting days elapsed */
uint8_t lastHour;

void rtcInit(void)
{
	twiMasterInit(100000);
	lastHour = 0;
	sei();
}

void rtcUpdate(void)
{
	lastHour = rtcData.hours;
	uint8_t trash;
	ds1307GetTime(&rtcData.hours, &rtcData.minutes, &rtcData.seconds, &trash);
	ds1307GetDate(&rtcData.year, &rtcData.month, &rtcData.day, &trash);
}

