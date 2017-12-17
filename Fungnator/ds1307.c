/* -----------------------------------------------------------------------------
 * Project:			GPDSE AVR8 Library
 * File:			ds1307.c
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

// -----------------------------------------------------------------------------
// Header files ----------------------------------------------------------------

#include "ds1307.h"
#if __DS1307_H != 80
	#error Error 101 - Version mismatch on header and source code files (ds1307).
#endif

// -----------------------------------------------------------------------------
// Global variables ------------------------------------------------------------

ds1307Configuration_t ds1307Configuration = {.hours = 0, .weekDay = 1, .amPm = DS1307_AM, .minutes = 0, .seconds = 0, .year = 0, .month = 1, .monthDay = 1, .squareWaveActivation = OFF, .squareWaveOffValue = LOW, .squareWaveFrequency = 0, .countingHalted = DS1307_COUNTING_RESUME, .timeFormat = DS1307_FORMAT_24_HOURS, .unusedBits = 0};

// -----------------------------------------------------------------------------
// Public function definitions -------------------------------------------------

/* -----------------------------------------------------------------------------
 * Function:	ds1307SetControl
 * Purpose:		Sets up the ds1307 control parameters
 * Arguments:	counting		Resumes or halts time counting (use enumerations
 *								at ds1307CountingStatus_t)
 *				squareWave		Square Wave Generator behavior (use enumerations
 *								at ds1307SquareWaveGenerator_t)
 *				format			Time format (use enumerations at
 *								ds1307TimeFormat_t)
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */
 
ds1307Result_t	ds1307SetControl(ds1307CountingStatus_t counting, ds1307SquareWaveGenerator_t squareWave, ds1307TimeFormat_t timeFormat)
{
	uint8 buffer[2];

	// Retrieve information from DS1307
	ds1307GetData();

	// Update internal struct
	if(counting != DS1307_COUNTING_NO_CHANGE)
		ds1307Configuration.countingHalted = counting;
	switch(squareWave){
	case DS1307_OUTPUT_LOW:
		ds1307Configuration.squareWaveActivation = OFF;
		ds1307Configuration.squareWaveOffValue = LOW;
		break;
	case DS1307_OUTPUT_HIGH:
		ds1307Configuration.squareWaveActivation = OFF;
		ds1307Configuration.squareWaveOffValue = HIGH;
		break;
	case DS1307_CLOCK_1HZ:
		ds1307Configuration.squareWaveActivation = ON;
		ds1307Configuration.squareWaveFrequency = 0;
		break;
	case DS1307_CLOCK_4KHZ:
		ds1307Configuration.squareWaveActivation = ON;
		ds1307Configuration.squareWaveFrequency = 1;
		break;
	case DS1307_CLOCK_8KHZ:
		ds1307Configuration.squareWaveActivation = ON;
		ds1307Configuration.squareWaveFrequency = 2;
		break;
	case DS1307_CLOCK_32KHZ:
		ds1307Configuration.squareWaveActivation = ON;
		ds1307Configuration.squareWaveFrequency = 3;
		break;
	default:		// DS1307_CLOCK_NO_CHANGE
		break;
	}
	if(timeFormat != DS1307_FORMAT_NO_CHANGE){
		// Need to change the time format
		if(timeFormat != ds1307Configuration.timeFormat){
			ds1307Configuration.timeFormat = timeFormat;
			if(timeFormat == DS1307_FORMAT_24_HOURS){	// Convert from AM/PM to 24 hours
				if(ds1307Configuration.amPm == DS1307_AM){
					if(ds1307Configuration.hours == 12)
						ds1307Configuration.hours = 0;
				}else{
					ds1307Configuration.hours += 12;
					if(ds1307Configuration.hours == 24)
						ds1307Configuration.hours = 12;
				}
			}else{										// Convert from 24 hours to AM/PM
				if(ds1307Configuration.hours >= 12){
					ds1307Configuration.amPm = DS1307_PM;
					ds1307Configuration.hours -= 12;
					if(ds1307Configuration.hours == 0)
						ds1307Configuration.hours = 12;
				}else{
					ds1307Configuration.amPm = DS1307_AM;
					if(ds1307Configuration.hours == 0)
						ds1307Configuration.hours = 12;
				}
			}
		}
	}

	// Send data back to DS1307
	buffer[0] = DS1307_ADDRESS_COUNTING;
	buffer[1] = (ds1307Configuration.seconds % 10);
	buffer[1] |= (ds1307Configuration.seconds / 10) << 4;
	buffer[1] |= ds1307Configuration.countingHalted << 7;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 2);
	buffer[0] = DS1307_ADDRESS_TIME_FORMAT;
	buffer[1] = (ds1307Configuration.hours % 10);
	buffer[1] |= (ds1307Configuration.hours / 10) << 4;
	if(ds1307Configuration.timeFormat == DS1307_FORMAT_AM_PM){
		buffer[1] |=  0x40;
		buffer[1] |= ds1307Configuration.amPm << 5;
	}
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 2);
	buffer[0] = DS1307_ADDRESS_CONTROL;
	buffer[1] = ds1307Configuration.squareWaveOffValue << 7;
	buffer[1] |= ds1307Configuration.squareWaveActivation << 4;
	buffer[1] |= ds1307Configuration.squareWaveFrequency << 0;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 2);

	return DS1307_OK;
}

/* -----------------------------------------------------------------------------
 * Function:	ds1307GetDate
 * Purpose:		Reads the current date from ds1307
 * Arguments:	year		Pointer to year variable
 *				month		Pointer to month variable
 *				monthDay	Pointer to day of the month variable
 *				weekDay		Pointer to day of the week variable
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */
 
ds1307Result_t	ds1307GetDate(uint8 * year, uint8 * month, uint8 * monthDay, uint8 * weekDay)
{
	uint8 buffer[4];

	// Sets DS1307 internal pointer to address 0x03
	buffer[0] = DS1307_ADDRESS_WEEK_DAY;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 1);
	// Retrieve all available data
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_READ, buffer, 4);
	twiMasterReadFromBuffer(buffer, 4);
	// Decodes the information
	ds1307Configuration.weekDay = buffer[0];
	ds1307Configuration.monthDay = ((buffer[1] >> 4) * 10) + (buffer[1] & 0x0F);
	ds1307Configuration.month = ((buffer[2] >> 4) * 10) + (buffer[2] & 0x0F);
	ds1307Configuration.year = ((buffer[3] >> 4) * 10) + (buffer[3] & 0x0F);
	*year = ds1307Configuration.year;
	*month = ds1307Configuration.month;
	*monthDay = ds1307Configuration.monthDay;
	*weekDay = ds1307Configuration.weekDay;

	return DS1307_OK;
}

/* -----------------------------------------------------------------------------
 * Function:	ds1307SetDate
 * Purpose:		Changes the current date at the ds1307
 * Arguments:	year		Value of the year
 *				month		Value of the month
 *				monthDay	Value of the monthDay
 *				weekDay		Value of the weekDay
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */
 
ds1307Result_t	ds1307SetDate(uint8 year, uint8 month, uint8 monthDay, uint8 weekDay)
{
	uint8 buffer[5];

	// Update internal struct
	ds1307Configuration.year = year;
	ds1307Configuration.month = month;
	ds1307Configuration.monthDay = monthDay;
	ds1307Configuration.weekDay = weekDay;

	// Send data back to DS1307
	buffer[0] = DS1307_ADDRESS_WEEK_DAY;
	buffer[1] = weekDay;
	buffer[2] = ((monthDay / 10) << 4) | (monthDay % 10);
	buffer[3] = ((month / 10) << 4) | (month % 10);
	buffer[4] = ((year / 10) << 4) | (year % 10);
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 5);

	return DS1307_OK;
}

/* -----------------------------------------------------------------------------
 * Function:	ds1307GetTime
 * Purpose:		Changes the current date at the ds1307
 * Arguments:	hours		Pointer to hours variable
 *				minutes		Pointer to minutes variable
 *				seconds		Pointer to seconds variable
 *				abbrev		Pointer to abbrviation suffix variable
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */

ds1307Result_t	ds1307GetTime(uint8 * hours, uint8 * minutes, uint8 * seconds, ds1307TimeAbbreviation_t * abbrev)
{
	uint8 buffer[3];

	// Sets DS1307 internal pointer to address 0x00
	buffer[0] = DS1307_ADDRESS_SECONDS;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 1);
	// Retrieve all available data
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_READ, buffer, 3);
	twiMasterReadFromBuffer(buffer, 3);
	// Decodes the information
	ds1307Configuration.countingHalted = isBitSet(buffer[0], 7);
	buffer[0] &= 0x7F;
	ds1307Configuration.seconds = ((buffer[0] >> 4) * 10) + (buffer[0] & 0x0F);
	ds1307Configuration.minutes = ((buffer[1] >> 4) * 10) + (buffer[1] & 0x0F);
	if(isBitSet(buffer[2], 6)){	// 12 hour-mode
		ds1307Configuration.timeFormat = DS1307_FORMAT_AM_PM;
		ds1307Configuration.amPm = isBitSet(buffer[2], 5);
		ds1307Configuration.hours = (((buffer[2] & 0x10) >> 4) * 10) + (buffer[2] & 0x0F);
	}else{						// 24 hour-mode
		ds1307Configuration.timeFormat = DS1307_FORMAT_24_HOURS;
		ds1307Configuration.hours = (((buffer[2] & 0x30) >> 4) * 10) + (buffer[2] & 0x0F);
	}
	
	*hours = ds1307Configuration.hours;
	*minutes = ds1307Configuration.minutes;
	*seconds = ds1307Configuration.seconds;
	if(ds1307Configuration.timeFormat == DS1307_FORMAT_24_HOURS){
		*abbrev = DS1307_24;
	}else{
		*abbrev = ds1307Configuration.amPm;
	}

	return DS1307_OK;
}

/* -----------------------------------------------------------------------------
 * Function:	ds1307SetTime
 * Purpose:		Changes the current date at the ds1307
 * Arguments:	hours		Pointer to hours variable
 *				minutes		Pointer to minutes variable
 *				seconds		Pointer to seconds variable
 *				abbrev		Pointer to abbrviation suffix variable
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */

ds1307Result_t	ds1307SetTime(uint8 hours, uint8 minutes, uint8 seconds, ds1307TimeAbbreviation_t abbrev)
{
	uint8 buffer[4];

	// Update internal struct
	ds1307Configuration.hours = hours;
	ds1307Configuration.minutes = minutes;
	ds1307Configuration.seconds = seconds;
	if(abbrev == DS1307_24){
		ds1307Configuration.timeFormat = DS1307_FORMAT_24_HOURS;
	}else{
		ds1307Configuration.timeFormat = DS1307_FORMAT_AM_PM;
		ds1307Configuration.amPm = abbrev;
	}

	// Send data back to DS1307
	buffer[0] = DS1307_ADDRESS_SECONDS;
	buffer[1] = ((seconds / 10) << 4) | (seconds % 10);
	buffer[1] |= ds1307Configuration.countingHalted << 7;
	buffer[2] = ((minutes / 10) << 4) | (minutes % 10);
	buffer[3] = ((hours / 10) << 4) | (hours % 10);
	if(abbrev != DS1307_24){
		buffer[3] |= 0x40;
		buffer[3] |= abbrev << 5;
	}
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 4);

	return DS1307_OK;
}

/* -----------------------------------------------------------------------------
 * Function:	ds1307SetRamData
 * Purpose:		Writes the given data at the posion of the internal RAM
 * Arguments:	data		Value to be stored in the internal RAM
 *				position	Position of the memory byte to be read (position
 *							values and absolute addresses are shifted 8 by an
 *							offset of 8 values)
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */

ds1307Result_t ds1307SetRamData(uint8 data, uint8 position)
{
	uint8 buffer[2];

	buffer[0] = DS1307_ADDRESS_RAM + position;
	buffer[1] = data;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 2);

	return DS1307_OK;
}

/* -----------------------------------------------------------------------------
 * Function:	ds1307GetRamData
 * Purpose:		Returns the data at the posion of the internal RAM
 * Arguments:	position	Position of the memory byte to be read (position
 *							values and absolute addresses are shifted 8 by an
 *							offset of 8 values)
 * Returns:		The value of the byte stores at the given position
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */

 uint8 ds1307GetRamData(uint8 position)
{
	uint8 buffer[1];

	buffer[0] = DS1307_ADDRESS_RAM + position;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 1);
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_READ, buffer, 1);
	twiMasterReadFromBuffer(buffer, 1);
	return buffer[0];
}

// -----------------------------------------------------------------------------
// Private function definitions ------------------------------------------------

/* -----------------------------------------------------------------------------
 * Function:	ds1307GetData
 * Purpose:		Gets data from DS1307 and updates internal struct
 * Arguments:	none
 * Returns:		DS1307_OK
 * Notes:		Since the i2c controller is interrupt-driven, this function must
 * 				be called outside of the interrupt vectors
 * -------------------------------------------------------------------------- */
 
ds1307Result_t ds1307GetData(void)
{
	uint8 buffer[8];

	// Sets DS1307 internal pointer to address 0x00
	buffer[0] = DS1307_ADDRESS_SECONDS;
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_WRITE, buffer, 1);
	// Retrieve all available data
	twiMasterSendData(DS1307_SLAVE_ADDRESS, TWI_MASTER_READ, buffer, 8);
	twiMasterReadFromBuffer(buffer, 8);
	// Decodes the information
	ds1307Configuration.countingHalted = isBitSet(buffer[0], 7);
	buffer[0] &= 0x7F;
	ds1307Configuration.seconds = ((buffer[0] >> 4) * 10) + (buffer[0] & 0x0F);
	ds1307Configuration.minutes = ((buffer[1] >> 4) * 10) + (buffer[1] & 0x0F);
	if(isBitSet(buffer[2], 6)){	// 12 hour-mode
		ds1307Configuration.timeFormat = DS1307_FORMAT_AM_PM;
		ds1307Configuration.amPm = isBitSet(buffer[2], 5);
		ds1307Configuration.hours = (((buffer[2] & 0x10) >> 4) * 10) + (buffer[2] & 0x0F);
	}else{						// 24 hour-mode
		ds1307Configuration.timeFormat = DS1307_FORMAT_24_HOURS;
		ds1307Configuration.hours = (((buffer[2] & 0x30) >> 4) * 10) + (buffer[2] & 0x0F);
	}
	ds1307Configuration.weekDay = buffer[3];
	ds1307Configuration.monthDay = ((buffer[4] >> 4) * 10) + (buffer[4] & 0x0F);
	ds1307Configuration.month = ((buffer[5] >> 4) * 10) + (buffer[5] & 0x0F);
	ds1307Configuration.year = ((buffer[6] >> 4) * 10) + (buffer[6] & 0x0F);
	ds1307Configuration.squareWaveOffValue = isBitSet(buffer[7], 7);
	ds1307Configuration.squareWaveActivation = isBitSet(buffer[7], 4);
	ds1307Configuration.squareWaveFrequency = buffer[7] & 0x03;
	
	return DS1307_OK;
}