#define VCC_OFFSET 0.05   // Offset for error correction in Volts. This can be 0.0 but usually is between -0.2 and +0.2 and is chip specific. 
                         
//===============================================================================
// Read the VCC voltage by referencing the 1.1v internal reference
// Useful for checking battery voltage.
// Value will never be greater than the 3.31V + VCC_OFFSET (which is the voltage Arduino is working on)
// The useable range for checking voltage is between 2.9v and 3.3v
float ReadVCC() 
{
  long result;
  long avgmv = 0;
  const int reps = 10;

  // Measure the voltage 10 times and take average
  for (int i = 1; i <= reps; i++ )
  {
    // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(10); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Enable the ADC
    while (bit_is_set(ADCSRA,ADSC));
    result = ADCL;
    result |= ADCH<<8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    avgmv+=result;
  }
   
  return (avgmv / reps / 1000.0) + VCC_OFFSET;
}

//===============================================================================
// Read the internal chip temperature by referencing the 1.1v internal reference voltage
float ReadTemp(void)
{
  unsigned int wADC;
  float t;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(10);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31 ) / 1.22;

  // The returned temperature is in degrees Celcius.
  return (t);
}
