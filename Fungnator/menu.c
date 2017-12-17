/*
 * menu.c
 *
 * Created: 11/27/2017 1:42:22 AM
 *  Author: Cleissom
 */

/* Header for all project specific definitions.
F_CPU, Times, Thresholds, Ports and others*/
#include "definitions.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "menu.h"

#include "lcd4_i2c.h"
#include "uart.h"
#include "actuators.h"
#include "system.h"

/* External variable that holds sensors values */
extern sensorData_t     sensorData;

/* External Variable for getting light status*/
extern actuatorStatus_t actuators;

/* External Variable for getting Cycle status*/
extern uint8_t			cycleStarted;

/* Variable that holds menu status */
menuFlags_t menuFlags;

extern initData_t EEMEM initData;


attachLcd(lcd);

void menuInit(void)
{
	lcdConfiguration(&lcd);
	lcdInit(&lcd, LCD_16X2, LCD_FONT_5X8);
	lcdStdio(&lcd);
	menuFlags.updateMenu = 1;
	menuFlags.menuActive = 1;
	printf("   FUNGNATOR\n            v1.0");
	_delay_ms(1000);
}

/*
This function prints on lcd based if cycle was already started and which menu is selected.
menu 0 gets data from "actuators.c" for showing light status.
*/
void updateMenu(void)
{
	if(menuFlags.menuTimeoutCount >= MENU_TIMEOUT) {
		menuFlags.menuTimeoutCount = 0;
		menuOff();
	}

	if(menuFlags.menuActivate) {
		menuOn();
		menuFlags.menuActivate = 0;
	}

	if(menuFlags.menuActive) {
		lcdClearScreen(&lcd);
		if(cycleStarted) {
			switch(menuFlags.actualMenu) {
			case 0:
				if(actuators.lightEnabled) {
					printf("T: %d%cC H: %d%%\nlight: on", sensorData.temperature, 0xDF, sensorData.humidity);
				} else {
					printf("T: %d%cC H: %d%%\nlight: off", sensorData.temperature, 0xDF, sensorData.humidity);
				}
				break;
			case 1:
				printf("time elapsed:\n%d days", eeprom_read_byte(&initData.daysElapsed));
				break;
			}
		} else {
			printf("      Cycle\n   Not Started");
		}
	}
}

void menuOn(void)
{
	menuFlags.menuActive = 1;
	lcdDisplay(&lcd, ON);
}

void menuOff(void)
{
	menuFlags.menuActive = 0;
	lcdDisplay(&lcd, OFF);
}

