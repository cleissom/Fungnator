/*
 * menu.h
 *
 * Created: 11/27/2017 1:42:46 AM
 *  Author: Cleissom
 */


#ifndef MENU_H_
#define MENU_H_


/* New data type to hold menu status */
typedef union {
	struct {
		uint8_t menuActive   : 1;
		uint8_t menuActivate : 1;
		uint8_t updateMenu   : 1;
		uint8_t actualMenu   : 1;
		uint8_t menuTimeoutCount;
	};
} menuFlags_t;

void menuInit(void);

void updateMenu(void);

void menuOn(void);

void menuOff(void);

#endif /* MENU_H_ */