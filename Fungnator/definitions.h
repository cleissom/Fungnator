/*
 * definitions.h
 *
 * Created: 11/28/2017 5:42:27 PM
 *  Author: Cleissom
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_


#define TEMPERATURE_MAX				29
#define TEMPERATURE_MIN				27

#define HUMIDITY_MAX				90
#define HUMIDITY_MIN				85

#define LIGHT_START_HOUR			1
#define LIGHT_STOP_HOUR				20

#define FRESH_AIR_FAN_ACTIVE_TIME	1	// minutes
#define FRESH_AIR_FAN_PERIOD		2	// minutes

#define MENU_TIMEOUT				10


// PORTS, PINS, DDRS Definitions
#define DHT_PORT				PORTC
#define DHT_PIN					PINC
#define DHT_DDR					DDRC
#define DHT_BIT					0

#define HEATER_PORT				PORTD
#define HEATER_PIN				PIND
#define HEATER_DDR				DDRD
#define HEATER_BIT				5

#define LIGHT_PORT				PORTD
#define LIGHT_PIN				PIND
#define LIGHT_DDR				DDRD
#define LIGHT_BIT				6

#define FOGGER_PORT				PORTC
#define FOGGER_PIN				PINC
#define FOGGER_DDR				DDRC
#define FOGGER_BIT				3

#define FRESH_AIR_FAN_PORT		PORTC
#define FRESH_AIR_FAN_PIN		PINC
#define FRESH_AIR_FAN_DDR		DDRC
#define FRESH_AIR_FAN_BIT		2

#define MENU_PORT				PORTD
#define MENU_PIN				PIND
#define MENU_DDR				DDRD
#define MENU_BIT				3

#define LIGHT_BUTTON_PORT		PORTD
#define LIGHT_BUTTON_PIN		PIND
#define LIGHT_BUTTON_DDR		DDRD
#define LIGHT_BUTTON_BIT		4

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// DEBUG
//----------------------------------------------------------------------------------------------------
#define DEBUG_DDR  DDRB
#define DEBUG_PORT PORTB
#define DEBUG_BIT  0
#define DEBUG_ON (DEBUG_DDR |= (1 << DEBUG_BIT));
#define DEBUG_SET (DEBUG_PORT |= (1 << DEBUG_BIT));
#define DEBUG_CLEAR (DEBUG_PORT &= ~(1 << DEBUG_BIT));
//----------------------------------------------------------------------------------------------------


#endif /* DEFINITIONS_H_ */