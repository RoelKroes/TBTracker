
//===============================================================================
// Read an external voltage from an analog PIN by referencing the 1.1v internal reference  
// DO NOT CONNECT A HIGHER VOLTAGE THAN THE PIN IS RATED FOR (or use a voltage divider)
float ReadExternalVoltage()
{
  float internalV = 0.0;
  float externalV = 0.0;

  // Get a reference to compare the external voltage with
  internalV = ReadVCC();
  
  // Read the external voltage
  if (USE_EXTERNAL_VOLTAGE)
  {
    analogReference(DEFAULT);
    for (int i = 1; i <=5; i++)
    {
      externalV += analogRead(EXTERNALVOLTAGE_PIN);
    }
    externalV = externalV / 5;
    return ((internalV / SAMPLE_RES) * externalV * DIVIDER_RATIO)+EXT_OFFSET;
  }
  else
  {
    return 0.0;
  }
}

//===============================================================================
// Read the VCC voltage by referencing the 1.1v internal reference
// Useful for checking battery voltage.
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
