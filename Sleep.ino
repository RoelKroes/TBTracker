#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

volatile char sleepCnt;


// Let the Arduino go to sleep
void SleepNow() {

  // Send a message just to show we are about to sleep
  Serial.println("Going to sleep");
  Serial.flush();

  // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
  static byte prevADCSRA = ADCSRA;
  ADCSRA = 0;

  /* Set the type of sleep mode we want. Can be one of (in order of power saving):
   SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
   SLEEP_MODE_ADC
   SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
   SLEEP_MODE_EXT_STANDBY
   SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
   SLEEP_MODE_PWR_DOWN (Deep sleep)
   */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  while (sleepCnt < TIME_TO_SLEEP) {

    // Turn of Brown Out Detection (low voltage). This is automatically re-enabled upon timer interrupt
    sleep_bod_disable();

    // Ensure we can wake up again by first disabling interrupts (temporarily) so
    // the wakeISR does not run before we are asleep and then prevent interrupts,
    // and then defining the ISR (Interrupt Service Routine) to run when poked awake by the timer
    noInterrupts();

    // clear various "reset" flags
    MCUSR = 0;  // allow changes, disable reset
    WDTCSR = bit (WDCE) | bit(WDE); // set interrupt mode and an interval
    // WDTCSR = bit (WDIE) | bit(WDP2) | bit(WDP1); //| bit(WDP0);    // This is 1 sec
    WDTCSR = bit (WDIE) | bit(WDP3) | bit(WDP0);    // This is 8 sec
    wdt_reset();

    // Allow interrupts now
    interrupts();

    // And enter sleep mode as set above
    sleep_cpu();
  }

  // --------------------------------------------------------
  // µController is now asleep until woken up by an interrupt
  // --------------------------------------------------------

  // Prevent sleep mode, so we don't enter it again, except deliberately, by code
  sleep_disable();

  // Wakes up at this point when timer wakes up µC
  Serial.println("I'm awake!");

  // Reset sleep counter
  sleepCnt = 0;

  // Re-enable ADC if it was previously running
  ADCSRA = prevADCSRA;
}

// When WatchDog timer causes µC to wake it comes here
ISR (WDT_vect) {

  // Turn off watchdog, we don't want it to do anything (like resetting this sketch)
  wdt_disable();

  // Increment the WDT interrupt count
  sleepCnt++;

  // Now we continue running the main Loop() just after we went to sleep
}
