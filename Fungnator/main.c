/*
 * Fungnator.c
 *
 * Created: 11/26/2017 10:30:05 PM
 * Author : Cleissom
 */

/* Header for all project specific definitions.
F_CPU, Times, Thresholds, Ports and others*/
#include "definitions.h"

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Header for using rtc functions such as
rtc Initialization and Update*/
#include "rtc.h"

/* Header for using actuators functions such as
actuators Initialization and Update*/
#include "actuators.h"

/* Header for using menu functions such as
menu Initialization and Update*/
#include "menu.h"

/* Header for using functions that monitors Cycles*/
#include "system.h"

/* Header for using SDCard functions*/
#include "sd.h"

#ifdef DEBUG
#include <stdlib.h>
#include "uart.h"
#endif /* DEBUG */

/*New data type to hold last values from buttons*/
typedef volatile union {
	struct {
		uint8_t menu  : 1;
		uint8_t light : 1;
	};
	uint8_t allFlags;
} lastButton_t;

/* Global variable that holds last values from buttons */
lastButton_t lastButton;

/* Global variable that is set when Timer 1 had a Compare Interrupt.
 Used for indicating when the button is held time enough */
uint8_t	volatile timer1Interrupt;

/*Global variable that is set when there is a INT0 interrupt.
Used for indicate that all sensors and actuators need to be updated*/
uint8_t volatile updateAll;

/* External Variable for getting light status*/
extern actuatorStatus_t	actuators;

/* External Variable for getting menu status*/
extern menuFlags_t		menuFlags;

/* External Variable for getting Cycle status*/
extern uint8_t			cycleStarted;

int main(void)
{
	checkSystemInitStatus(); // Check if cycle already started

	sei();

// RTC First Time Initialization
//----------------------------------------------------------------------------------------------------
#ifdef INIT_RTC

#define RTC_DAY		05
#define RTC_MONTH	12
#define RTC_YEAR	17
#define RTC_HOURS	11
#define RTC_MINUTES	05

#include "ds1307.h"
	twiMasterInit(10000);
	ds1307SetControl(DS1307_COUNTING_RESUME, DS1307_CLOCK_1HZ, DS1307_FORMAT_24_HOURS);
	ds1307SetDate(RTC_YEAR, RTC_MONTH, RTC_DAY, 0);
	ds1307SetTime(RTC_HOURS, RTC_MINUTES, 0, DS1307_24);
#endif /*INIT_RTC*/
//----------------------------------------------------------------------------------------------------

	// Start buttons flags as if they were released
	lastButton.allFlags = 1;

	/* Timer 1 Configuration
	 Timer 1 is used for counting how much time button "Menu" is pressed.
	 When the specific time is met, timer1Interrupt flag is set.
	 Counter is cleared every time the button is released */
	TCCR1B |= (1 << WGM12);	 // Mode CTC with top on OCR1A
	OCR1A = 0xB71A;			 // 3 sec timeout
	TIMSK1 |= (1 << OCIE1A); // Output Compare A Match Interrupt Enable
	TIFR1  |= (1 << OCF1A);  // Clear Compare A Flag

	/* Buttons Initialization */
	MENU_PORT         |= (1 << MENU_BIT);	// Initializes Menu Button
	LIGHT_BUTTON_PORT |= (1 << LIGHT_BUTTON_BIT); // Initializes Light Button
	PCICR |= (1 << PCIE2);	// Pin Change Interrupt Enable 2
	PCMSK2 |= (1 << MENU_BIT) | (1 << LIGHT_BUTTON_BIT); // Masks Menu Button and Light Button for Interruption
	PCIFR  |= (1 << PCIF2); // Clear Pin Change Interrupt Flag 2

	// INT0 Configuration
	EICRA |= (1 << ISC01); // Interrupt on falling edge
	EIMSK |= (1 << INT0);  // External Interrupt Request 0 Enable
	EIFR  |= (1 << INTF0); // Clear interrupt flag

	SMCR |= (1 << SE); // Sleep Enable

#ifdef DEBUG
	uart_init(UART_BAUD_SELECT(9600, F_CPU));
#endif /* DEBUG */

	checkSd(); // Check if SDCard has already a ongoing file

	// Initialize Modules
	rtcInit();
	rtcUpdate(); // First Update after initialization
	actuatorsInit();
	menuInit();

	while(1) {

		if(updateAll) {
			updateAll = 0; // reset flag

			// Update Modules
			rtcUpdate();
			actuatorsUpdate();
			updateDaysElapsed();
			writeDataSd();

			// when timer1Interrupt is set, toggle Cycle status
			if(timer1Interrupt) {
				if(cycleStarted) {
					stopCycle();
					terminateSd();
				} else {
					startCycle();
					initSd(); // A new Cycle creates a new file on SDCard
				}
				timer1Interrupt = 0;
				menuFlags.updateMenu = 1;
			}

			updateMenu(); // Update Changes on Menu

		}

		sleep_cpu(); // sleep until next INT0 interrupt
	}
}

// External Interrupt 0 Handler
ISR(INT0_vect)
{
	updateAll = 1; // Set that all sensors and actuators need to be updated

	// Add counter for turning menu off
	if(menuFlags.menuActive) {
		menuFlags.menuTimeoutCount++;
	}
}

//Pin Change Interrupt Handler
ISR(PCINT2_vect)
{
	_delay_ms(30); // Debouncing


	if((lastButton.menu) && ((MENU_PIN & (1 << MENU_BIT)) == 0)) {
		// Execute this when Menu button is pressed

		// Menu alteration
		if(menuFlags.actualMenu == 0) {
			menuFlags.actualMenu = 1;
		} else {
			menuFlags.actualMenu = 0;
		}

		// Start counting button hold time
		TCNT1 = 0; // Clear Timer 1 Counter
		TCCR1B |= (1 << CS12) | (1 << CS10); // Start Timer with Prescaler: 1024
		TIFR1  |= (1 << OCF1A); // Clear OCR1A Flag

	} else {
		// Stop counting button hold time
		TCNT1 = 0; // Clear Timer 1 Counter
		TCCR1B &= ~((1 << CS12) | (1 << CS10)); // Stop Timer with Prescaler: 1024
		TIFR1  |= (1 << OCF1A); // Clear OCR1A Flag
	}


	if((lastButton.light) && ((LIGHT_BUTTON_PIN & (1 << LIGHT_BUTTON_BIT)) == 0)) {
		// Execute this when Light button is pressed

		// Toggle Light status
		if(actuators.lightEnabled == 0) {
			actuators.lightEnabled = 1;
		} else {
			actuators.lightEnabled = 0;
		}
	}

	lastButton.menu = ((MENU_PIN & (1 << MENU_BIT)) >> MENU_BIT); // Get last menu button value
	lastButton.light = ((LIGHT_BUTTON_PIN & (1 << LIGHT_BUTTON_BIT)) >> LIGHT_BUTTON_BIT); // Get last light button value
	menuFlags.updateMenu = 1;

	// If menu is Off, turn it on
	if(menuFlags.menuActive == 0) {
		menuFlags.menuActivate = 1;
	}

	// Clear Counting Flag
	menuFlags.menuTimeoutCount = 0;
}

// Timer 1 Interrupt Handler
ISR(TIMER1_COMPA_vect)
{
	timer1Interrupt = 1;
}