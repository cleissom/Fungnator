#ifndef SPI_H
#define SPI_H

#include "globalDefines.h"
#include <avr/io.h>

//Inicializa a SPI
void SPI_Init(void);

//Envia e recebe um byte via SPI
char SPI_SendReceiveByte(char data);

//Envia um byte via SPI e ignora o que recebe
void SPI_SendByte(char data);

//Envia um byte "dummy" via SPI e retorna o que recebeu
char SPI_ReceiveByte(void);

#endif
