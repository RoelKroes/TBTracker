#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

int teller = 0;

//===============================================================================
void setup_Sleep(void)
{  
  // Setup the watchdog timer to run an interrupt which
  // wakes the Arduino from sleep every 1 second.
  
  // Note that the default behavior of resetting the Arduino
  // with the watchdog will be disabled.
  
  // This next section of code is timing critical, so interrupts are disabled.
  // See more details of how to change the watchdog in the ATmega328P datasheet
  // around page 50, Watchdog Timer.
  noInterrupts();
  
  // Set the watchdog reset bit in the MCU status register to 0.
  MCUSR &= ~(1<<WDRF);
  
  // Set WDCE and WDE bits in the watchdog control register.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // Set watchdog clock prescaler bits to a value of 8 seconds.
  // Use this if you want intervals of 8 secs instead of 1 secs
  // WDTCSR = (1<<WDP0) | (1<<WDP3);
  
  // Set watchdog clock prescaler bits to a value of 1 second
  WDTCSR = bit (WDIE) | bit(WDP2) | bit(WDP1); //| bit(WDP0);    
  
  // Enable watchdog as interrupt only (no reset).
  WDTCSR |= (1<<WDIE);
  
  // Enable interrupts again.
  interrupts();
}


//===============================================================================
// Put the Arduino to sleep.
void my_Sleep()
{
  setup_Sleep();
  // Set sleep to full power down.  Only external interrupts or 
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off the ADC while asleep.
  power_adc_disable();

  // Enable sleep and enter sleep mode.
  sleep_mode();

  // CPU is now asleep and program execution completely halts!
  // Once awake, execution will resume at this point.
  
  // When awake, disable sleep mode and turn on all devices.
  sleep_disable();
  power_all_enable();
}

//===============================================================================
// Define watchdog timer interrupt.
ISR(WDT_vect)
{
  // Set the watchdog activated flag.
  // Note that you shouldn't do much work inside an interrupt handler.
  watchdogActivated = true;
  sleepIterations++;
}
