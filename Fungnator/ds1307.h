/* -----------------------------------------------------------------------------
 * Project:			GPDSE AVR8 Library
 * File:			ds1307.h
 * Module:			DS1307 Real Time Clock controller
 * Author:			Leandro Schwarz
 * Version:			8.0
 * Last edition:	2016-01-25
 * Purpose:			Controls and interfaces a DS1307 Real Time Clock
 * Notes:			This code makes use of some funcions defined in twiMaster
 *					module. To use this code with the dedicated peripheral,
 *					just include the twiMaster module files. In order to use the
 *					device with a I2C bitbang interface, please use the same
 *					function names.
 * -------------------------------------------------------------------------- */

#ifndef __DS1307_H
#define __DS1307_H 80

// -----------------------------------------------------------------------------
// Header files ----------------------------------------------------------------

#include "globalDefines.h"
#if __GLOBALDEFINES_H < 123
	#error Error 100 - The defintion file is outdated (globalDefines must be version 12.3 or greater).
#endif
#include "twiMaster.h"

// -----------------------------------------------------------------------------
// Constant definitions --------------------------------------------------------

#define DS1307_SLAVE_ADDRESS		0x68
#define DS1307_ADDRESS_SECONDS		0x00
#define DS1307_ADDRESS_COUNTING		0x00
#define DS1307_ADDRESS_MINUTES		0x01
#define DS1307_ADDRESS_HOURS		0x02
#define DS1307_ADDRESS_TIME_FORMAT	0x02
#define DS1307_ADDRESS_WEEK_DAY		0x03
#define DS1307_ADDRESS_MONTH_DAY	0x04
#define DS1307_ADDRESS_MONTH		0x05
#define DS1307_ADDRESS_YEAR			0x06
#define DS1307_ADDRESS_CONTROL		0x07
#define DS1307_ADDRESS_RAM			0x08
#define DS1307_I2C_MIN_CLOCK_SPEED	0UL
#define DS1307_I2C_MAX_CLOCK_SPEED	100000UL

#ifndef TWI_MASTER_READ
	#define TWI_MASTER_READ				1
#endif
#ifndef TWI_MASTER_WRITE
	#define TWI_MASTER_WRITE			0
#endif

// -----------------------------------------------------------------------------
// New data types --------------------------------------------------------------

typedef struct ds1307Configuration_t{
	uint8 hours					: 5;
	uint8 weekDay				: 3;
	uint8 amPm					: 1;
	uint8 minutes				: 6;
	uint8 seconds				: 6;
	uint8 year					: 7;
	uint8 month					: 4;
	uint8 monthDay				: 5;
	uint8 squareWaveActivation	: 1;
	uint8 squareWaveOffValue	: 1;
	uint8 squareWaveFrequency	: 2;
	uint8 countingHalted		: 1;
	uint8 timeFormat			: 1;
	uint8 unusedBits			: 5;
} ds1307Configuration_t;

typedef enum ds1307Result_t{
	DS1307_OK					= 0
} ds1307Result_t;

typedef enum ds1307CountingStatus_t{
	DS1307_COUNTING_RESUME	= 0,
	DS1307_COUNTING_HALT,
	DS1307_COUNTING_NO_CHANGE = 255
} ds1307CountingStatus_t;

typedef enum ds1307SquareWaveGenerator_t{
	DS1307_OUTPUT_LOW = 0,
	DS1307_OUTPUT_HIGH,
	DS1307_CLOCK_1HZ,
	DS1307_CLOCK_4KHZ,
	DS1307_CLOCK_8KHZ,
	DS1307_CLOCK_32KHZ,
	DS1307_CLOCK_NO_CHANGE = 255
} ds1307SquareWaveGenerator_t;

typedef enum ds1307TimeFormat_t{
	DS1307_FORMAT_24_HOURS = 0,
	DS1307_FORMAT_AM_PM,
	DS1307_FORMAT_NO_CHANGE = 255
} ds1307TimeFormat_t;

typedef enum ds1307TimeAbbreviation_t{
	DS1307_AM = 0,
	DS1307_PM,
	DS1307_24
} ds1307TimeAbbreviation_t;

// -----------------------------------------------------------------------------
// Global variables ------------------------------------------------------------

extern ds1307Configuration_t ds1307Configuration;

// -----------------------------------------------------------------------------
// Public functions declaration ------------------------------------------------

ds1307Result_t	ds1307SetControl(ds1307CountingStatus_t counting, ds1307SquareWaveGenerator_t squareWave, ds1307TimeFormat_t timeFormat);
ds1307Result_t	ds1307GetDate(uint8 * year, uint8 * month, uint8 * monthDay, uint8 * weekDay);
ds1307Result_t	ds1307SetDate(uint8 year, uint8 month, uint8 monthDay, uint8 weekDay);
ds1307Result_t	ds1307GetTime(uint8 * hours, uint8 * minutes, uint8 * seconds, ds1307TimeAbbreviation_t * abbrev);
ds1307Result_t	ds1307SetTime(uint8 hour, uint8 minute, uint8 second, ds1307TimeAbbreviation_t abbrev);
uint8			ds1307GetRamData(uint8 position);
ds1307Result_t	ds1307SetRamData(uint8 data, uint8 position);

// -----------------------------------------------------------------------------
// Private functions declaration - do not use outside this module --------------

ds1307Result_t ds1307GetData(void);

#endif