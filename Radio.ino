// include the library
#include <RadioLib.h>

#define PACKETLEN 255

// Change 'SX1278' in the line below to 'SX1276' if you have a SX1276 module.
SX1278 radio = new Module(PIN_NSS, PIN_DIO0, PIN_DIO1);

// create RTTY client instance using the radio module
RTTYClient rtty(&radio);


//===============================================================================
void SetupRTTY()
{

  // First setup FSK
  SetupFSK();
#if defined(DEVMODE)          
  Serial.print(F("RTTY init.."));
#endif
  int16_t state = rtty.begin(RTTYSettings.Frequency,
                     RTTYSettings.Shift,
                     RTTYSettings.Baud,
                     RTTYSettings.Encoding,
                     RTTYSettings.StopBits  );
                     
  if(state == RADIOLIB_ERR_NONE) // Change this to (state == ERR_NONE) if you use an older radiolib library
  {
#if defined(DEVMODE)            
    Serial.println(F("done"));
#endif
  } else 
  {
#if defined(DEVMODE)
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif
    while(true);
  }
}


//===============================================================================
void SetupFSK()
{
  // Reset the radio
  ResetRadio();
  // Initialize the SX1278
#if defined(DEVMODE)  
  Serial.print(F("[SX1278] init.."));
#endif

 // int16_t state = radio.beginFSK();
 
  int16_t state = radio.beginFSK(FSKSettings.Frequency,
                               FSKSettings.BitRate,
                               FSKSettings.FreqDev,
                               FSKSettings.RXBandwidth,
                               FSKSettings.Power,
                               FSKSettings.PreambleLength,
                               FSKSettings.EnableOOK);


  if(state == RADIOLIB_ERR_NONE) // Change this to (state == ERR_NONE) if you use an older radiolib library
  {
#if defined(DEVMODE)    
    Serial.println(F("done"));
#endif
  } 
  else 
  {
#if defined(DEVMODE)    
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif
    while(true);
  }
}


//===============================================================================
void SetupLoRa()
{
  // Initialize the SX1278
#if defined(DEVMODE)  
  Serial.print(F("[LoRA] Initializing ... "));
#endif

  ResetRadio();
  // First setup the mode
  // 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on - NOT IMPLEMENTED YET
  // 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off - NOT IMPLEMENTED YET
  
  // 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off - Should work correctly
  // 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off - Should work correctly
  switch (LORA_MODE)
  {
    case 0: 
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 20.8;
      LoRaSettings.SpreadFactor = 11;
      break;   

    case 1:
      LoRaSettings.CodeRate = 5;
      LoRaSettings.Bandwidth = 20.8;      
      LoRaSettings.SpreadFactor = 6;      
    break;   
    
    case 2:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 62.5;      
      LoRaSettings.SpreadFactor = 8;      
      break;   

    case 3:
      LoRaSettings.CodeRate = 6;
      LoRaSettings.Bandwidth = 250;      
      LoRaSettings.SpreadFactor = 7;            
      break;   
    case 5:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 41.7;      
      LoRaSettings.SpreadFactor = 11;            
    break;
    
  }
  
  int16_t state = radio.begin
  (
    LoRaSettings.Frequency,
    LoRaSettings.Bandwidth,
    LoRaSettings.SpreadFactor,
    LoRaSettings.CodeRate,
    LoRaSettings.SyncWord,
    LoRaSettings.Power,
    LoRaSettings.PreambleLength, 
    LoRaSettings.Gain
  );
  
  switch(LORA_MODE) 
  {
    case 0:
      radio.forceLDRO(true);
      radio.setCRC(true);  
    break;
    case 1:
      radio.implicitHeader(PACKETLEN);
      //radio.forceLDRO(true);
      radio.setCRC(true);
    break;
    default:
      radio.explicitHeader();
      //radio.autoLDRO();
      radio.setCRC(true);
    break;
  }  
  
  if(state == RADIOLIB_ERR_NONE) // Change this to (state == ERR_NONE) if you use an older radiolib library
  {
#if defined(DEVMODE)    
    Serial.println(F("done"));
#endif    
  } 
  else 
  {
#if defined(DEVMODE)    
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif    
    while(true);
  }
}


//===============================================================================
void ResetRadio()
{
  // Use for ESP based boards
  /*
  pinMode(PIN_RESET,OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  delay(100);
  digitalWrite(PIN_RESET,HIGH);
  delay(100);
  */
}


//===============================================================================
void SetupRadio()
{
  // Setting up the radio
  if (RTTY_ENABLED) {SetupRTTY();}
  if (LORA_ENABLED) {SetupLoRa();}
}

//===============================================================================
void sendRTTY(String TxLine)
{
   // Disable the GPS on the softwareserial temporarily 
   SerialGPS.end();
   
   SetupRTTY();
   
   // Send only idle carrier to let people get their tuning right
   rtty.idle();     
   delay(RTTY_IDLE_TIME);

   // Send the string 
#if defined(DEVMODE)   
   Serial.print(F("Send RTTY: "));
   Serial.println(TxLine);
#endif   
   
   int state = rtty.println(TxLine); 
   rtty.standby();   
   // Enable the GPS again.  
   SerialGPS.begin(GPSBaud);
}


//===============================================================================
void sendLoRa(String TxLine)
{
   int state;

   SetupLoRa();

#if defined(DEVMODE)            
    Serial.println(TxLine);
#endif      

  switch (LORA_MODE)
  {
    case 1:
      int i;
      int j; 
      // Send the string 
      char buf[PACKETLEN];
      for (j=0; j<PACKETLEN; j++) { buf[j] = '\0';}
      for (i=0; i<TxLine.length(); i++) {buf[i] = TxLine[i];}
      state = radio.transmit((uint8_t*)buf,PACKETLEN);       
    break;
    default:
      // Send the string 
      state = radio.transmit(TxLine); 
    break;      
  }   

  if(state == RADIOLIB_ERR_NONE) // Change this to (state == ERR_NONE) if you use an older radiolib library
  {
#if defined(DEVMODE)            
    Serial.println(F("done"));
#endif
  } else 
  {
#if defined(DEVMODE)
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif 
  } 
}
