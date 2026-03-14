#pragma once

#include <stdint.h>
#include <avr/io.h>

#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_PIN		PINB

#define SPI_SCK		PB5
#define SPI_MISO	PB4
#define SPI_MOSI	PB3
#define SPI_SS		PB2
#define SPI_RST     PB1

static inline void setSSPinLow(void) {
    SPI_PORT &= ~(1 << SPI_SS);
}

static inline void setSSPinHigh(void) {
    SPI_PORT |= (1 << SPI_SS);
}

void spiInitBus(void); // Initialization of the SPI interface of MCU
uint8_t spiTransferByte(uint8_t byteOfData); // Puts (reads) one byte on (from) a bus
