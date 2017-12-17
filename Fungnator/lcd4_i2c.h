/* -----------------------------------------------------------------------------
 * Project:			GPDSE AVR8 Library
 * File:			lcd4d.h
 * Module:			Character LCD controller
 * Author:			Leandro Schwarz
 * Version:			1.1
 * Last edition:	2016-01-15
 * Purpose:			Character LCD controller with 4 bits interface without
 *					support to busy flag (delay-driven)
 * -------------------------------------------------------------------------- */

#ifndef __LCD4_I2C_H
#define __LCD4_I2C_H 11

// -----------------------------------------------------------------------------
// Header files ----------------------------------------------------------------

#include "twimaster.h"
#include "globalDefines.h"
#if __GLOBALDEFINES_H < 123
#error Error 100 - The defintion file is outdated (globalDefines must be version 12.3 or greater).
#endif
#include <stdio.h>

// -----------------------------------------------------------------------------
// Constant definitions --------------------------------------------------------

#define PCF8574A_ADR		0x3F
#define LCD_DATAFIRST		4
#define LCD_E				2
#define LCD_RW				5	// sempre em baixo
#define LCD_RS				0
#define LCD_i2c_start		0x08

#define LCD_CLEAR_SCREEN			0x01
#define LCD_CURSOR_HOME				0x02
#define LCD_ENTRY_SET				0x04
#define LCD_ENTRY_INCREMENT		0x02
#define LCD_ENTRY_DECREMENT		0x00
#define LCD_ENTRY_SHIFT			0x01
#define LCD_ENTRY_OVERWRITE		0x00
#define LCD_DISPLAY_OFF				0x08
#define LCD_DISPLAY_ON			0x0C
#define LCD_FUNCTION_RESET			0x30
#define LCD_FUNCTION_SET			0x20
#define LCD_FUNCTION_8_BITS		0x10
#define LCD_FUNCTION_4_BITS		0x00
#define LCD_FUNCTION_2_LINES	0x08
#define LCD_FUNCTION_1_LINE		0x00
#define LCD_FUNCTION_5x10_FONT	0x04
#define LCD_FUNCTION_5x8_FONT	0x00
#define LCD_CGRAM_ADRESS			0x40
#define LCD_DDRAM_ADRESS			0x80

// -----------------------------------------------------------------------------
// New data types --------------------------------------------------------------

typedef struct lcdConfiguration_t {
	// I/O ports
	uint8 i2c_data;
	uint8 dataFirst				: 3;
	uint8 controlRS				: 3;
	uint8 controlE				: 3;
	// Display hardware
	uint8 size					: 5;
	uint8 functionFontSize		: 1;	// 0 5x8, 1 5x10
	// Entry mode set
	uint8 entryIncDec			: 1;
	uint8 entryShiftDisplay		: 1;
	// Display and cursor configuration
	uint8 displayOn				: 1;	// 0 off, 1 on
	uint8 cursorOn				: 1;	// 0 off, 1 on
	uint8 cursorBlink			: 1;	// 0 off, 1 on
	uint8 cursorColumn			: 6;	// 0 to 39
	uint8 cursorLine			: 2;	// 0 to 3
	// Initialization flags
	uint8 initialized			: 1;
	// Unused bits
	uint8 unusedBits			: 1;
} lcdConfiguration_t;

typedef enum lcdSize_t {
	LCD_SIZE_UNDEFINED = 0,
	LCD_8X1 = 1,
	LCD_8X2,
	LCD_10x2,
	LCD_12X2,
	LCD_12X4,
	LCD_16X1,
	LCD_16X1b,
	LCD_16X2,
	LCD_16X4,
	LCD_20X1,
	LCD_20X2,
	LCD_20X4,
	LCD_24X1,
	LCD_24X2,
	LCD_30X2,
	LCD_32X2,
	LCD_40X1,
	LCD_40X2
} lcdSize_t;

typedef enum lcdFont_t {
	LCD_FONT_5X8 = 0,
	LCD_FONT_5X10 = 1
} lcdFont_t;

typedef enum lcdShiftOverwrite_t {
	LCD_DISPLAY_OVERWRITE = 0,
	LCD_DISPLAY_SHIFT = 1,
	LCD_DISPLAY_NO_CHANGE = 0xFF
} lcdShiftOverwrite_t;

typedef enum lcdIncrementDecrement_t {
	LCD_DECREMENT = 0,
	LCD_INCREMENT = 1,
	LCD_INCDEC_NO_CHANGE = 0xFF
} lcdIncrementDecrement_t;

typedef enum lcdResult_t {
	LCD_OK = 0,
	LCD_UNSUPPORTED_FONT_SIZE,
	LCD_UNSUPPORTED_INTERFACE_MODE,
	LCD_UNSUPPORTED_DISPLAY_SIZE,
	LCD_UNSUPPORTED_PARAMETER,
	LCD_DATA_PORT_NOT_CONFIGURED,
	LCD_CONTROL_PORT_NOT_CONFIGURED,
	LCD_NOT_INITIALIZED
} lcdResult_t;

// -----------------------------------------------------------------------------
// Global variables ------------------------------------------------------------

extern FILE lcdStream;
extern volatile lcdConfiguration_t * defaultDisplay;

// -----------------------------------------------------------------------------
// Macrofunctions --------------------------------------------------------------

#define attachLcd(object) volatile lcdConfiguration_t object;

// -----------------------------------------------------------------------------
// Public functions declaration ------------------------------------------------

lcdResult_t lcdInit(volatile lcdConfiguration_t * lcd, lcdSize_t size, lcdFont_t font);
lcdResult_t lcdStdio(volatile lcdConfiguration_t * lcd);
lcdResult_t lcdConfiguration(volatile lcdConfiguration_t * lcd);
lcdResult_t lcdClearScreen(volatile lcdConfiguration_t * lcd);
lcdResult_t lcdCursorHome(volatile lcdConfiguration_t * lcd);
lcdResult_t lcdDisplay(volatile lcdConfiguration_t * lcd, logic_t state);
lcdResult_t lcdCursor(volatile lcdConfiguration_t * lcd, logic_t state);
lcdResult_t lcdCursorBlink(volatile lcdConfiguration_t * lcd, logic_t state);
lcdResult_t lcdSetEntryMode(volatile lcdConfiguration_t * lcd, lcdIncrementDecrement_t dir, lcdShiftOverwrite_t mode);
lcdResult_t lcdCursorGoTo(volatile lcdConfiguration_t * lcd, uint8 line, uint8 column);
lcdResult_t lcdCursorMoveNextLine(volatile lcdConfiguration_t * lcd);
lcdResult_t lcdCursorMoveFirstLine(volatile lcdConfiguration_t * lcd);
lcdResult_t lcdCursorMove(volatile lcdConfiguration_t * lcd, direction_t dir);
lcdResult_t lcdDisplayShift(volatile lcdConfiguration_t * lcd, direction_t dir);

// -----------------------------------------------------------------------------
// Private functions declaration - do not use outside this module --------------

lcdResult_t lcdWriteCommand(volatile lcdConfiguration_t * lcd, uint8 command);
lcdResult_t lcdFunctionSet8Bits(volatile lcdConfiguration_t * lcd, uint8 command);
lcdResult_t lcdWriteCharacter(volatile lcdConfiguration_t * lcd, uint8 character);
int16	lcdWriteStd(int8 c, FILE * stream);

#endif