/*
 * system.h
 *
 * Created: 11/29/2017 7:51:29 PM
 *  Author: Cleissom
 */


#ifndef SYSTEM_H_
#define SYSTEM_H_

/* New data type to hold Cycle data */
typedef volatile struct {
	uint8_t cycleStarted;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t daysElapsed;
} initData_t;

void checkSystemInitStatus(void);

void startCycle(void);

void stopCycle(void);

void updateDaysElapsed(void);

#endif /* SYSTEM_H_ */