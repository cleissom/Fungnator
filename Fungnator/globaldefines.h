/* -----------------------------------------------------------------------------
 * Project:			GPDSE AVR8 Integrated Library
 * File:			globalDefines.h
 * Module:			Global definitions file for the GAIL Project
 * Author:			Leandro Schwarz
 * Version:			12.3
 * Last edition:	2016-01-15
 * ---------------------------------------------------------------------------*/

#ifndef __GLOBALDEFINES_H
#define __GLOBALDEFINES_H 123

// -----------------------------------------------------------------------------
// Basic definitions -----------------------------------------------------------

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// -----------------------------------------------------------------------------
// Header files ----------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// Constant definitions --------------------------------------------------------

// -----------------------------------------------------------------------------
// Bit handling macro functions ------------------------------------------------

#ifndef setBit
#define setBit(reg, bit)					((reg) |= (1 << (bit)))
#endif
#ifndef clrBit
#define clrBit(reg, bit)					((reg) &= ~(1 << (bit)))
#endif
#ifndef cplBit
#define cplBit(reg, bit)					((reg) ^= (1 << (bit)))
#endif
#ifndef isBitSet
#define isBitSet(reg, bit)					(((reg) >> (bit)) & 1)
#endif
#ifndef isBitClr
#define isBitClr(reg, bit)					(!(((reg) >> (bit)) & 1))
#endif
#ifndef waitUntilBitIsSet
#define waitUntilBitIsSet(reg, bit)			do{}while(isBitClr((reg), (bit)))
#endif
#ifndef waitUntilBitIsClear
#define waitUntilBitIsClear(reg, bit)		do{}while(isBitSet((reg), (bit)))
#endif
#ifndef noOperation
#define noOperation(cycles)					__builtin_avr_delay_cycles(cycles)
#endif
#ifndef setMask
#define setMask(reg, mask, offset)			((reg) |= ((mask) << (offset)))
#endif
#ifndef clrMask
#define clrMask(reg, mask, offset)			((reg) &= ~((mask) << (offset)))
#endif
#ifndef cplMask
#define cplMask(reg, mask, offset)			((reg) ^= ((mask) << (offset)))
#endif

// -----------------------------------------------------------------------------
// New data types --------------------------------------------------------------

typedef char				int8;
typedef int					int16;
typedef long int			int32;
typedef long long			int64;
typedef unsigned char		uint8;
typedef unsigned int		uint16;
typedef unsigned long int	uint32;
typedef unsigned long long	uint64;

typedef enum bool_t {
	FALSE = 0,
	TRUE = 1
} bool_t;

typedef enum logic_t {
	LOW = 0,
	HIGH = 1,
	OFF = 0,
	ON = 1
} logic_t;

typedef enum direction_t {
	LEFT = 0,
	RIGHT = 1,
	UP = 0,
	DOWN = 1,
	CLOCKWISE = 0,
	COUNTERCLOCKWISE = 1
} direction_t;

typedef enum resultValue_t {
	RESULT_OK = 0,
	RESULT_PORT_VALUE_UNSUPPORTED,
	RESULT_INT_SENSE_VALUE_UNSUPPORTED,
	RESULT_TIMER_PRESCALER_VALUE_UNSUPPORTED,
	RESULT_TIMER_MODE_VALUE_UNSUPPORTED,

	RESULT_USART_PARITY_UNSUPORTED,
	RESULT_USART_BAUD_RATE_UNSUPORTED,
	RESULT_UNSUPPORTED_VALUE
} resultValue_t;

typedef enum usartError_t {
	USART_OK = 1,
	USART_FRAME_ERROR = 2,
	USART_PARITY_ERROR  = 4,
	USART_BUFFER_OVERFLOW_ERROR = 8
} usartError_t;

typedef enum systemPrescalerValue_t {
	SYSTEM_PRESCALER_OFF = 0,
	SYSTEM_PRESCALER_2 = 1,
	SYSTEM_PRESCALER_4 = 2,
	SYSTEM_PRESCALER_8 = 3,
	SYSTEM_PRESCALER_16 = 4,
	SYSTEM_PRESCALER_32 = 5,
	SYSTEM_PRESCALER_64 = 6,
	SYSTEM_PRESCALER_128 = 7,
	SYSTEM_PRESCALER_256 = 8
} systemPrescalerValue_t;

typedef enum eepromMode_t {
	EEPROM_ERASE_WRITE = 0,
	EEPROM_ERASE_ONLY,
	EEPROM_WITE_ONLY
} eepromMode_t;

typedef enum portMode_t {
	PORT_OUTPUT_LOW = 0,
	PORT_OUTPUT_HIGH,
	PORT_INPUT_TRISTATE,
	PORT_INPUT_PULL_UP,
	PORT_NO_CHANGE = 255
} portMode_t;

typedef enum senseMode_t {
	SENSE_LOW_LEVEL = 0,
	SENSE_ANY_EDGE,
	SENSE_FALLING_EDGE,
	SENSE_RISING_EDGE,
	SENSE_NO_CHANGE = 255
} senseMode_t;

typedef enum timerModeA_t {
	TIMER_A_MODE_NORMAL = 0,
	TIMER_A_MODE_PWM_PHASE_CORRECTED_MAX = 1,
	TIMER_A_MODE_CTC = 2,
	TIMER_A_MODE_FAST_PWM_MAX = 3,
	TIMER_A_MODE_PWM_PHASE_CORRECTED_OCRA = 5,
	TIMER_A_MODE_FAST_PWM_OCRA = 7,
	TIMER_A_MODE_NO_CHANGE = 255
} timerModeA_t;

typedef enum timerModeB_t {
	TIMER_B_MODE_NORMAL = 0,
	TIMER_B_MODE_PWM_PHASE_CORRECTED_8_BITS = 1,
	TIMER_B_MODE_PWM_PHASE_CORRECTED_9_BITS = 2,
	TIMER_B_MODE_PWM_PHASE_CORRECTED_10_BITS = 3,
	TIMER_B_MODE_CTC = 4,
	TIMER_B_MODE_FAST_PWM_8_BITS = 5,
	TIMER_B_MODE_FAST_PWM_9_BITS = 6,
	TIMER_B_MODE_FAST_PWM_10_BITS = 7,
	TIMER_B_MODE_PWM_PHASE_FREQUENCY_CORRECTED_ICR = 8,
	TIMER_B_MODE_PWM_PHASE_FREQUENCY_CORRECTED_OCRA = 9,
	TIMER_B_MODE_PWM_PHASE_CORRECTED_ICR = 10,
	TIMER_B_MODE_PWM_PHASE_CORRECTED_OCRA = 11,
	TIMER_B_MODE_CTC_ICR = 12,
	TIMER_B_MODE_FAST_PWM_ICR = 14,
	TIMER_B_MODE_FAST_PWM_OCRA = 15,
	TIMER_B_MODE_NO_CHANGE = 255
} timerModeB_t;

typedef enum timerPrescalerValueA_t {
	TIMER_A_CLOCK_DISABLE = 0,
	TIMER_A_PRESCALER_OFF = 1,
	TIMER_A_PRESCALER_8 = 2,
	TIMER_A_PRESCALER_64 = 3,
	TIMER_A_PRESCALER_256 = 4,
	TIMER_A_PRESCALER_1024 = 5,
	TIMER_A_PRESCALER_T0_FALLING_EDGE = 6,
	TIMER_A_PRESCALER_T0_RISING_EDGE = 7,
	TIMER_A_PRESCALER_NO_CHANGE = 255
} timerPrescalerValueA_t;

typedef enum timerPrescalerValueB_t {
	TIMER_B_CLOCK_DISABLE = 0,
	TIMER_B_PRESCALER_OFF = 1,
	TIMER_B_PRESCALER_8 = 2,
	TIMER_B_PRESCALER_32 = 3,
	TIMER_B_PRESCALER_64 = 4,
	TIMER_B_PRESCALER_128 = 5,
	TIMER_B_PRESCALER_256 = 6,
	TIMER_B_PRESCALER_1024 = 7,
	TIMER_B_PRESCALER_NO_CHANGE = 255
} timerPrescalerValueB_t;

typedef enum timerOutput_t {
	TIMER_PORT_NORMAL = 0,
	TIMER_PORT_TOGGLE_ON_COMPARE = 1,
	TIMER_PORT_CLEAR_ON_COMPARE = 2,
	TIMER_PORT_SET_ON_COMPARE = 3,
	TIMER_PORT_NON_INVERTING_MODE = 2,
	TIMER_PORT_INVERTING_MODE = 3,
	TIMER_PORT_NO_CHANGE = 255
} timerOutput_t;

typedef enum timerInputCaptureNoiseCanceler_t {
	TIMER_NOISE_CANCELER_OFF = 0,
	TIMER_NOISE_CANCELER_FALLING_EDGE = 1,
	TIMER_NOISE_CANCELER_RISING_EDGE = 3
} timerInputCaptureNoiseCanceler_t;

typedef enum adcPrescaler_t {
	ADC_PRESCALER_OFF = 0,
	ADC_PRESCALER_2 = 1,
	ADC_PRESCALER_4 = 2,
	ADC_PRESCALER_8 = 3,
	ADC_PRESCALER_16 = 4,
	ADC_PRESCALER_32 = 5,
	ADC_PRESCALER_64 = 6,
	ADC_PRESCALER_128 = 7,
	ADC_PRESCALER_NO_CHANGE = 255
} adcPrescaler_t;

typedef enum adcReference_t {
	ADC_REFERENCE_EXTERNAL = 0,
	ADC_REFERENCE_POWER_SUPPLY = 1,
	ADC_REFERENCE_INTERNAL = 3,
	ADC_REFERENCE_NO_CHANGE = 255
} adcReference_t;

typedef enum adcChannel_t {
	ADC_CHANNEL_0 = 0,
	ADC_CHANNEL_1 = 1,
	ADC_CHANNEL_2 = 2,
	ADC_CHANNEL_3 = 3,
	ADC_CHANNEL_4 = 4,
	ADC_CHANNEL_5 = 5,
	ADC_CHANNEL_6 = 6,
	ADC_CHANNEL_7 = 7,
	ADC_CHANNEL_TEMPERATURE = 8,
	ADC_CHANNEL_BAND_GAP = 14,
	ADC_CHANNEL_GND = 15,
	ADC_CHANNEL_NO_CHANGE = 255
} adcChannel_t;

typedef enum adcMode_t {
	ADC_MODE_SINGLE_CONVERSION = 0,
	ADC_MODE_AUTO_CONTINUOUS,
	ADC_MODE_AUTO_ANALOG_COMP,
	ADC_MODE_AUTO_INT0,
	ADC_MODE_AUTO_TIMER0_COMPA,
	ADC_MODE_AUTO_TIMER0_OVERFLOW,
	ADC_MODE_AUTO_TIMER1_COMPB,
	ADC_MODE_AUTO_TIMER1_OVERFLOW,
	ADC_MODE_AUTO_TIMER1_CAPTURE,
	ADC_MODE_NO_CHANGE = 255
} adcMode_t;

typedef enum adcDataPresentation_t {
	ADC_LEFT_ADJUSTED = 0,
	ADC_RIGHT_ADJUSTED
} adcDataPresentation_t;

typedef enum usartDataBits_t {
	USART_DATA_BITS_5 = 0,
	USART_DATA_BITS_6,
	USART_DATA_BITS_7,
	USART_DATA_BITS_8,
	USART_DATA_BITS_9,
	USART_DATA_BITS_NO_CHANGE = 255
} usartDataBits_t;

typedef enum usartStopBits_t {
	USART_STOP_BIT_SINGLE = 0,
	USART_STOP_BIT_DOUBLE,
	USART_STOP_BIT_NO_CHANGE = 255
} usartStopBits_t;

typedef enum usartParity_t {
	USART_PARITY_NONE = 0,
	USART_PARITY_ODD,
	USART_PARITY_EVEN,
	USART_PARITY_MARK,
	USART_PARITY_SPACE,
	USART_PARITY_NO_CHANGE = 255
} usartParity_t;

typedef enum usartMode_t {
	USART_MODE_ASYNCHRONOUS = 0,
	USART_MODE_ASYNCHRONOUS_DOUBLE_SPEED,
	USART_MODE_SYNCHRONOUS_XCK_FALLING,
	USART_MODE_SYNCHRONOUS_XCK_RISING,
	USART_MODE_SYNCHRONOUS_SPI,
	USART_MODE_NO_CHANGE = 255
} usartMode_t;

typedef enum usartBaudRate_t {
	USART_BAUD_600 = 600UL,
	USART_BAUD_1200 = 1200UL,
	USART_BAUD_2400 = 2400UL,
	USART_BAUD_4800 = 4800UL,
	USART_BAUD_9600 = 9600UL,
	USART_BAUD_14400 = 14400UL,
	USART_BAUD_19200 = 19200UL,
	USART_BAUD_28800 = 28800UL,
	USART_BAUD_38400 = 38400UL,
	USART_BAUD_56000 = 56000UL,
	USART_BAUD_57600 = 57600UL,
	USART_BAUD_115200 = 115200UL,
	USART_BAUD_128000 = 128000UL,
	USART_BAUD_256000 = 256000UL,
	USART_BAUD_NO_CHANGE = 0xFFFFFFFFUL
} usartBaudRate_t;

#endif