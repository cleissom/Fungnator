/*
 * actuators.h
 *
 * Created: 11/29/2017 2:22:56 PM
 *  Author: Cleissom
 */


#ifndef ACTUATORS_H_
#define ACTUATORS_H_

/*New data type to sensors data*/
typedef struct {
	int16_t temperature;
	int16_t humidity;
} sensorData_t;

/*New data type to hold Actuators Status*/
typedef struct {
	union {
		struct {
			uint8_t heaterStatus     : 1;
			uint8_t foggerStatus	  : 1;
			uint8_t lightEnabled	  : 1;
			uint8_t lightStatus       : 1;
			uint8_t freshAirFanStatus : 1;
		};
		uint8_t allStatus;
	};

	/* Variables that holds Fresh Air Fan Times*/
	uint8_t freshAirFanStartHours;
	uint8_t freshAirFanStartMinutes;
	uint8_t freshAirFanStopHours;
	uint8_t freshAirFanStopMinutes;
} actuatorStatus_t; // 1 if active

void actuatorsInit(void);

void actuatorsUpdate(void);

void lightEnable(void);

void lightDisable(void);

#endif /* ACTUATORS_H_ */