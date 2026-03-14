#pragma once

#include <stdbool.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define wdtReset() __asm__ __volatile__ ("wdr")

// WDT timeout values
typedef enum {
    WDT_TIMEOUT_17MS = 0,
    WDT_TIMEOUT_34MS,
    WDT_TIMEOUT_68MS,
    WDT_TIMEOUT_140MS,
    WDT_TIMEOUT_270MS,
    WDT_TIMEOUT_550MS,
    WDT_TIMEOUT_1S,
    WDT_TIMEOUT_2S,
} wdt_timeout_t;

void wdtEnable(void);
void wdtDisable(bool disableInterrupts);
void wdtSetTimeout(wdt_timeout_t timeout, bool disableInterrupts);
