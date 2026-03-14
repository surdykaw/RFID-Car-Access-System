#include "wdt-config.h"

void wdtEnable() {
  wdtReset();
  /* Clear watchdog reset flag in MCUSR */
  MCUCSR &= ~(1<<WDRF);
  // Enable configuration change
  WDTCR |= (1<<WDCE) | (1<<WDE);
}

void wdtDisable(bool disableInterrupts) {
  if(disableInterrupts)
    cli();

  wdtReset();

  /* Clear WDRF in MCUSR */
  MCUCSR &= ~(1<<WDRF);
  /* Write logical one to WDCE and WDE */
  /* Keep old prescaler setting to prevent unintentional time-out
  */
  WDTCR |= (1<<WDCE) | (1<<WDE);
  /* Turn off WDT */
  WDTCR = 0x00;

  if(disableInterrupts)
    sei();
}

void wdtSetTimeout(wdt_timeout_t timeout, bool disableInterrupts) {
  if(disableInterrupts)
    cli();

  wdtReset();
  /* Start timed sequence */
  WDTCR |= (1<<WDCE) | (1<<WDE);
  /* Set new time-out */
  WDTCR = (1 << WDE) | (timeout & 0x07);
  
  if(disableInterrupts)
    sei();
}
