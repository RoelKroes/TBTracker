
// include the library
#include <RadioLib.h>


// SX1278 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// DIO2 pin:  3 // not existent set to -1 and not necessary for this project
// BUSY pin:  9
// Reset Pin is in pin 7
SX1278 fsk = new Module(PIN_NSS, PIN_DIO0, PIN_DIO1);

// create RTTY client instance using the FSK module
RTTYClient rtty(&fsk);


//===============================================================================
void SetupRTTY()
{

  // First setup FSK
  SetupFSK();
  
  Serial.print(F("[RTTY] Initializing ... "));
  int16_t state = rtty.begin(RTTYSettings.Frequency,
                     RTTYSettings.Shift,
                     RTTYSettings.Baud,
                     RTTYSettings.Encoding,
                     RTTYSettings.StopBits  );
                     
  if(state == ERR_NONE) 
  {
    Serial.println(F("success!"));
  } else 
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }
}


//===============================================================================
void SetupFSK()
{
  // Reset the radio
  ResetRadio();
  // Initialize the SX1278
  Serial.print(F("[SX1278] Initializing ... "));

 // int16_t state = fsk.beginFSK();
 
  int16_t state = fsk.beginFSK(FSKSettings.Frequency,
                               FSKSettings.BitRate,
                               FSKSettings.FreqDev,
                               FSKSettings.RXBandwidth,
                               FSKSettings.Power,
                               FSKSettings.CurrentLimit,
                               FSKSettings.PreambleLength,
                               FSKSettings.EnableOOK);


  if(state == ERR_NONE) 
  {
    Serial.println(F("success!"));
  } 
  else 
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }
}


//===============================================================================
void SetupLoRa()
{
  // Initialize the SX1278
  Serial.print(F("[LoRA] Initializing ... "));

  ResetRadio();
  // First setup the mode
  // 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on
  // 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off
  // 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off
  // 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off
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
  }
  
  int16_t state = fsk.begin
  (
    LoRaSettings.Frequency,
    LoRaSettings.Bandwidth,
    LoRaSettings.SpreadFactor,
    LoRaSettings.CodeRate,
    LoRaSettings.SyncWord,
    LoRaSettings.Power,
    LoRaSettings.CurrentLimit,
    LoRaSettings.PreambleLength, 
    LoRaSettings.Gain
  );
  
  if(state == ERR_NONE) 
  {
    Serial.println(F("success!"));
  } 
  else 
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
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
  SetupRTTY();
  SetupLoRa();
}

//===============================================================================
void sendRTTY(String TxLine)
{
   // Disable the GPS on the softwareserial temporarily 
   SerialGPS.end();
   
   SetupRTTY();
   
   // Idle for 5 secs
   rtty.idle();     
   delay(5000);
   
   // Send the string 
   Serial.print(F("Sending RTTY: "));

   Serial.println(TxLine);
   
   int state = rtty.println(TxLine); 

   // Enable the GPS again.  
   SerialGPS.begin(GPSBaud);
}


//===============================================================================
void sendLoRa(String TxLine)
{
   SetupLoRa();
   
   Serial.print(F("Sending LoRa: "));
   Serial.println(TxLine);
   
   // Send the string
   int state = fsk.transmit(TxLine); 
}
