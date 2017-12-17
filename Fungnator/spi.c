#include "spi.h"

//Inicializa a SPI
void SPI_Init(void)
{
	//PORTB = 0xFF;
	DDRB = (1<<PB2) | (1<<PB3) | (1<<PB5);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPI2X)| (1<<SPR1) | (1<<SPR0);
}

//Envia e recebe um byte via SPI
char SPI_SendReceiveByte(char data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

//Envia um byte via SPI e ignora o que recebe
void SPI_SendByte(char data)
{
	SPI_SendReceiveByte(data);
}

//Envia um byte "dummy" via SPI e retorna o que recebeu
char SPI_ReceiveByte(void)
{
	return SPI_SendReceiveByte(0xFF);
}
