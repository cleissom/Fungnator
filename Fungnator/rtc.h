/*
 * rtc.h
 *
 * Created: 11/28/2017 11:44:05 PM
 *  Author: Cleissom
 */


#ifndef RTC_H_
#define RTC_H_

/* New data type to hold rtc data */
typedef volatile struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} rtcData_t;

void rtcInit(void);

void rtcUpdate(void);

#endif /* RTC_H_ */