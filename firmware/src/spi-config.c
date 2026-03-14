#include "spi-config.h"

void spiInitBus(void) {
//	Set MOSI and SCK output, all others input
	SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS) | (1 << SPI_RST);
	SPI_DDR &= ~(1 << SPI_MISO);
	SPI_PORT |= (1 << SPI_MISO);
	SPI_PORT |= (1 << SPI_RST);
//	Enable SPI, Master, set clock rate fck/16
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}
uint8_t spiTransferByte(uint8_t byteOfData) {
//	Start transmission
	SPDR = byteOfData;
//	Wait for transmission complete
	while(!(SPSR & (1 << SPIF)));

	return SPDR; // Read received data
}