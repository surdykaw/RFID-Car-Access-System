#include <avr/io.h>
#include <util/delay.h>

#include "spi-config.h"
#include "mfrc522.h"
#include "wdt-config.h"
#include "system-config.h"

void setRegisters(void) {
	// DATA REGISTERS
	DDRB |= 0x0F;
	PORTB &= ~(0x01);
	DDRC |= 0x3F;
	PORTC = 0x00;
	DDRD = 0xFF;
	PORTD = 0x00;

	// MINIMIZING POWER CONSUMPTION
	ADCSRA &= ~(1 << ADEN); // Analog - Digital Converter
	ACSR &= ~(1 << ACIE);
	ACSR |= (1 << ACD); // Analog comparator
	UCSRB = 0x00; // USART Control Register
}

uint8_t validateTag(uint8_t *uid) {
    for(uint8_t tag = 0; tag < TAG_COUNT; tag++) {
        uint8_t match = 1;

        for (uint8_t i = 0; i < 4; i++) {
            if (TAGS_UID_TAB[tag][i] != uid[i]) {
                match = 0;
                break;
            }
        }
        if(match)
            return ACCESS;
    }
    return NO_ACCESS;
}

void lock() {
	PORTC |= (1 << LOCK);
	_delay_ms(1000);
	PORTC &= ~(1 << LOCK);
}

void unlock() {
	PORTC |= (1 << UNLOCK);
	_delay_ms(1000);
	PORTC &= ~(1 << UNLOCK);
}

void clearArrays(uint8_t* tagUID, uint8_t* responseFIFO) {
	for (uint8_t i = 0; i < 4; i++) {
		tagUID[i] = 0x00;
	}
	for (uint8_t i = 0; i < FIFO_BUFFER_SIZE; i++) {
		responseFIFO[i] = 0x00;
	}
}

int main(void) {
    uint8_t status;
    uint8_t tagUID[4] = {0x00};
    bool locked = true;
    uint8_t noAccessCounter = 0;
    uint8_t errorOccurred = 0;

    /* watchdog setup */
    wdtDisable(true);
    wdtEnable();
    wdtSetTimeout(WDT_TIMEOUT_2S, true);

	setRegisters();
    _delay_ms(500);

    PORTC |= (1 << INFO_LED);

    /* SPI init */
    spiInitBus();
    _delay_ms(1000);
    wdtReset();

    /* RFID init */
    mfrc522Initialize();
    _delay_ms(1000);

    PORTC &= ~(1 << INFO_LED);
    wdtReset();

    while(true) {
        status = mfrc522GetTagUID(tagUID);
        wdtReset();

        if(status == CORRECT_UID) {
        	if(validateTag(tagUID) == ACCESS) {
                PORTC |= (1 << INFO_LED);
                locked = !locked;

                if(locked)
                    lock();
                else
                    unlock();

                PORTC &= ~(1 << INFO_LED);
                noAccessCounter = 0;
                errorOccurred = 0;
                wdtReset();
            }
            else {
                noAccessCounter++;
                if(noAccessCounter > 2) {
                    noAccessCounter = 0;
                    PORTC |= (1 << INFO_LED);
                    /* 1 minute timeout */
                    for(uint8_t i = 0; i < 60; i++) {
                        _delay_ms(1000);
                        wdtReset();
                    }
                    PORTC &= ~(1 << INFO_LED);
                }
            }
            clearArrays(tagUID, rfidBuffer);
        }
        else if(status == ERROR) { // Complete reset when ERROR occurred twice
            PORTC |= (1 << INFO_LED);
            clearArrays(tagUID, rfidBuffer);
            
			if(!errorOccurred)
                errorOccurred = 1;
            else {
                errorOccurred = 0;
                /* full reader reset */
                do {
                    mfrc522HardReset();
                    _delay_ms(200);
                    spiInitBus();
                    _delay_ms(200);
                    mfrc522Initialize();
                    status = mfrc522GetTagUID(tagUID);
                } while(status == ERROR);
                wdtReset();
            }
            PORTC &= ~(1 << INFO_LED);
        }
        _delay_ms(1000);
    }

    return 0;
}