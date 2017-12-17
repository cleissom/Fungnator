/*
 * actuators.c
 *
 * Created: 11/28/2017 5:43:57 PM
 *  Author: Cleissom
 */

/* Header for all project specific definitions.
F_CPU, Times, Thresholds, Ports and others*/
#include "definitions.h"

#include <avr/io.h>
#include <util/delay.h>

#include "actuators.h"

/* humidity and temperature sensors library */
#include "dhtxx.h"

/* Header for getting rtc data */
#include "rtc.h"

/* Header for getting Cycle status */
#include "system.h"

#ifdef DEBUG
#include <stdlib.h>
#include "uart.h"
#endif /* DEBUG */

// Variable that holds actuators status
actuatorStatus_t actuators;

// Variable that holds sensors values
sensorData_t     sensorData;

// External variable that holds rtc data
extern rtcData_t rtcData;

/* External Variable for getting Cycle status*/
extern uint8_t cycleStarted;

void actuatorsInit(void)
{
	actuators.allStatus = 0;

	actuators.freshAirFanStartHours = rtcData.hours;
	actuators.freshAirFanStartMinutes = rtcData.minutes;

	actuators.freshAirFanStopHours = actuators.freshAirFanStartHours + ((uint8_t)((uint16_t)FRESH_AIR_FAN_ACTIVE_TIME / 60));

	actuators.freshAirFanStopMinutes = actuators.freshAirFanStartMinutes + (uint8_t)((uint16_t)FRESH_AIR_FAN_ACTIVE_TIME - (((uint16_t)FRESH_AIR_FAN_ACTIVE_TIME / 60) * 60));
	if(actuators.freshAirFanStopMinutes >= 60) {
		actuators.freshAirFanStopHours += (actuators.freshAirFanStopMinutes / 60);
		actuators.freshAirFanStopMinutes = (actuators.freshAirFanStopMinutes - 60);
	}

	if(actuators.freshAirFanStopHours >= 24) {
		actuators.freshAirFanStopHours = (actuators.freshAirFanStopHours - 24);
	}


	HEATER_DDR       |= (1 << HEATER_BIT);
	LIGHT_DDR         |= (1 << LIGHT_BIT);
	FOGGER_DDR        |= (1 << FOGGER_BIT);
	FRESH_AIR_FAN_DDR |= (1 << FRESH_AIR_FAN_BIT);

	dhtxxconvert(DHTXX_DHT22, &DHT_PORT, &DHT_DDR, &DHT_PIN, (1 << DHT_BIT));

	_delay_ms(1000);

	return;
}

void actuatorsUpdate(void)
{
	if(cycleStarted) {

		// Get sensor data
		dhtxxread(DHTXX_DHT22, &DHT_PORT, &DHT_DDR, &DHT_PIN, (1 << DHT_BIT), &(sensorData.temperature), &(sensorData.humidity));
		sensorData.temperature /= 10;
		sensorData.humidity /= 10;


#ifdef DEBUG
// Sends rtc actual data

		int8_t q[10];
		itoa(rtcData.hours, q, 10);
		uart_puts("H:");
		uart_puts(q);
		uart_putc('\r');

		itoa(rtcData.minutes, q, 10);
		uart_puts("M:");
		uart_puts(q);
		uart_putc('\r');

		itoa(rtcData.seconds, q, 10);
		uart_puts("S:");
		uart_puts(q);
		uart_putc('\r');

		itoa(rtcData.day, q, 10);
		uart_puts("d:");
		uart_puts(q);
		uart_putc('\r');

		itoa(rtcData.month, q, 10);
		uart_puts("m:");
		uart_puts(q);
		uart_putc('\r');

		itoa(rtcData.year, q, 10);
		uart_puts("y:");
		uart_puts(q);
		uart_putc('\r');
		uart_putc('\r');
#endif /* DEBUG */

		// If temperature is below threshold and heater is not active, activate it
		if((sensorData.temperature <= TEMPERATURE_MIN) && ((actuators.heaterStatus) == 0)) {
			HEATER_PORT |= (1 << HEATER_BIT);
			actuators.heaterStatus = 1;
		} else if((sensorData.temperature >= TEMPERATURE_MAX) && (actuators.heaterStatus)) {
			HEATER_PORT &= ~(1 << HEATER_BIT);
			actuators.heaterStatus = 0;
		}

		// If humidity is below threshold and fogger is not active, activate it
		if((sensorData.humidity < HUMIDITY_MIN) && (!(actuators.foggerStatus))) {
			FOGGER_PORT     |= (1 << FOGGER_BIT);
			actuators.foggerStatus = 1;
		} else if((sensorData.humidity > HUMIDITY_MAX) && (actuators.foggerStatus)) {
			FOGGER_PORT     &= ~(1 << FOGGER_BIT);
			actuators.foggerStatus = 0;
		}


		// Fresh Air Fan Handler
		if((rtcData.hours >= actuators.freshAirFanStartHours) &&
		        (rtcData.minutes >= actuators.freshAirFanStartMinutes) &&
		        (actuators.freshAirFanStatus == 0)) {
			FRESH_AIR_FAN_PORT |= (1 << FRESH_AIR_FAN_BIT);
			actuators.freshAirFanStatus = 1;
		} else if(((((rtcData.hours < actuators.freshAirFanStartHours) &&
		             (rtcData.minutes < actuators.freshAirFanStartMinutes) &&
		             (rtcData.hours < actuators.freshAirFanStopHours) &&
		             rtcData.minutes < actuators.freshAirFanStopMinutes) &&
		            (actuators.freshAirFanStartHours >= actuators.freshAirFanStopHours)) ||
		           ((rtcData.hours >= actuators.freshAirFanStopHours) &&
		            (rtcData.minutes >= actuators.freshAirFanStopMinutes) &&
		            (actuators.freshAirFanStopHours >= actuators.freshAirFanStartHours)) ||
		           ((rtcData.hours >= actuators.freshAirFanStopHours) &&
		            (rtcData.minutes >= actuators.freshAirFanStopMinutes) &&
		            (rtcData.hours < actuators.freshAirFanStartHours) &&
		            (rtcData.minutes < actuators.freshAirFanStartMinutes))) &&
		          (actuators.freshAirFanStatus == 1)) {
			FRESH_AIR_FAN_PORT &= ~(1 << FRESH_AIR_FAN_BIT);
			actuators.freshAirFanStatus = 0;
			actuators.freshAirFanStartMinutes += (uint8_t)((uint16_t)FRESH_AIR_FAN_PERIOD - (((uint16_t)FRESH_AIR_FAN_PERIOD / 60) * 60));
			if(actuators.freshAirFanStartMinutes >= 60) {
				actuators.freshAirFanStartHours += (actuators.freshAirFanStartMinutes / 60);
				actuators.freshAirFanStartMinutes = (actuators.freshAirFanStartMinutes - 60);
			}
			actuators.freshAirFanStartHours += (uint8_t)((uint16_t)FRESH_AIR_FAN_PERIOD / 60);
			if(actuators.freshAirFanStartHours >= 24) {
				actuators.freshAirFanStartHours = (actuators.freshAirFanStartHours - 24);
			}
			actuators.freshAirFanStopMinutes = actuators.freshAirFanStartMinutes + (uint8_t)((uint16_t)FRESH_AIR_FAN_ACTIVE_TIME - (((uint16_t)FRESH_AIR_FAN_ACTIVE_TIME / 60) * 60));
			if(actuators.freshAirFanStopMinutes >= 60) {
				actuators.freshAirFanStopHours += (actuators.freshAirFanStopMinutes / 60);
				actuators.freshAirFanStopMinutes = (actuators.freshAirFanStopMinutes - 60);
			}
			actuators.freshAirFanStopHours = actuators.freshAirFanStartHours + ((uint8_t)((uint16_t)FRESH_AIR_FAN_ACTIVE_TIME / 60));
			if(actuators.freshAirFanStopHours >= 24) {
				actuators.freshAirFanStopHours = (actuators.freshAirFanStopHours - 24);
			}
		}

#ifdef DEBUG
		// Sends Fresh Air Fan actual times
		itoa(actuators.freshAirFanStartHours, q, 10);
		uart_puts("Start Hours:");
		uart_puts(q);
		uart_putc('\r');

		itoa(actuators.freshAirFanStartMinutes, q, 10);
		uart_puts("Start Minutes:");
		uart_puts(q);
		uart_putc('\r');

		itoa(actuators.freshAirFanStopHours, q, 10);
		uart_puts("Stop Hours:");
		uart_puts(q);
		uart_putc('\r');

		itoa(actuators.freshAirFanStopMinutes, q, 10);
		uart_puts("Stop Minutes:");
		uart_puts(q);
		uart_putc('\r');
		uart_putc('\r');
#endif /* DEBUG */

		// If light is enabled, turn it on only when its on time
		if(actuators.lightEnabled) {
			if(((rtcData.hours >= LIGHT_START_HOUR) && (rtcData.hours < LIGHT_STOP_HOUR)) && (actuators.lightStatus == 0)) {
				LIGHT_PORT |= (1 << LIGHT_BIT);
				actuators.lightStatus = 1;
			} else if(((rtcData.hours < LIGHT_START_HOUR) || (rtcData.hours >= LIGHT_STOP_HOUR)) && (actuators.lightStatus == 1)) {
				LIGHT_PORT &= ~(1 << LIGHT_BIT);
				actuators.lightStatus = 0;
			}
		} else if((actuators.lightStatus == 1) && (actuators.lightEnabled == 0)) {
			LIGHT_PORT &= ~(1 << LIGHT_BIT);
			actuators.lightStatus = 0;
		}


	} else {
		// If Cycle is not started, turn all off.
		HEATER_PORT &= ~(1 << HEATER_BIT);
		actuators.heaterStatus = 0;
		FOGGER_PORT     &= ~(1 << FOGGER_BIT);
		actuators.foggerStatus = 0;
		FRESH_AIR_FAN_PORT &= ~(1 << FRESH_AIR_FAN_BIT);
		actuators.freshAirFanStatus = 0;
		LIGHT_PORT &= ~(1 << LIGHT_BIT);
		actuators.lightStatus = 0;
	}
}

void lightEnable(void)
{
	actuators.lightEnabled = 1;
}

void lightDisable(void)
{
	actuators.lightEnabled = 0;
}