#include <SoftwareSerial.h>
#include "settings.h"

/***********************************************************************************
 *  FIRST THING YOU NEED TO DO IS ADJUST THE SETTINGS IN settings.h
 *  
 *  Have FUN!
 ***********************************************************************************/


/***********************************************************************************
 * LoRa and RTTY tracker for Arduino Mini and SX1278
 * Specially designed for the cheap TTGO T-deer board
 * a TTGO T-Deer board is an Arduino Mini at 3.3v with a SX1278 LoRa chip on board
 * 
 * by Roel Kroes
 * roel@kroes.com
 *
 *  
 *  To run this, you need a TTGO T-Deer board or:
 *  1 x ARduino Mini - 3.3v (or compatible Arduino board)
 *  1 x SX1278 LoRa chip (or compatible LoRa chip)
 *  1 x BN220 GPS (Ublox compatible / 9600 Baud) or any compatible GPS device
 *    
 *  BN22 <> Arduino Mini
 *  VCC -> 3.3V (red wirre)
 *  GND -> GND (black wire)
 *  RX -> D7 (white wire)
 *  TX -> D8 (green wire)
 *  
 *  Extra librairies needed:
 *  https://github.com/jgromes/RadioLib (Radiolib)
 *  https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)
 *  
 *  For payload information and how to get your see the file Misc.ini from this sketch
 ************************************************************************************/

// Struct to hold GPS data
struct TGPS
{
  int Hours, Minutes, Seconds;
  float Longitude, Latitude;
  long Altitude;
  unsigned int Satellites;
  byte FlightMode;
} UGPS;

// Struct to hold LoRA settings
struct TLoRaSettings
{
  float Frequency = LORA_FREQUENCY;
  float Bandwidth = LORA_BANDWIDTH;
  uint8_t SpreadFactor = LORA_SPREADFACTOR;
  uint8_t CodeRate = LORA_CODERATE;
  uint8_t SyncWord = LORA_SYNCWORD;
  uint8_t Power = LORA_POWER;
  uint8_t CurrentLimit = LORA_CURRENTLIMIT;
  uint16_t PreambleLength =  LORA_PREAMBLELENGTH;
  uint8_t Gain = LORA_GAIN;
} LoRaSettings;

// Struct to hold FSK settings
struct TFSKSettings
{
  float Frequency = FSK_FREQUENCY;
  float BitRate = FSK_BITRATE; 
  float FreqDev = FSK_FREQDEV;
  float RXBandwidth = FSK_RXBANDWIDTH;
  int8_t  Power = FSK_POWER;                  // in dbM range 2 - 17
  uint8_t CurrentLimit = FSK_CURRENTLIMIT;    // in mA
  uint16_t  PreambleLength = FSK_PREAMBLELENGTH;
  bool  EnableOOK = FSK_ENABLEOOK;
  float dataShaping = FSK_DATASHAPING;
} FSKSettings;

// Struct to hold RTTY settings
struct TRTTYSettings
{
  float Frequency = RTTY_FREQUENCY;   // Base frequency
  uint32_t Shift = RTTY_SHIFT;        // RTTY shift
  uint16_t Baud = RTTY_BAUD;          // Baud rate
  uint8_t Encoding = RTTY_ASCII;   // Encoding (ASCII = 7 bits)
  uint8_t StopBits = RTTY_STOPBITS;   // Number of stopbits 
} RTTYSettings;

/***********************************************************************************
* GLOBALS
*  
* Normally no change necessary
************************************************************************************/
SoftwareSerial SerialGPS(Rx, Tx);
char Sentence[SENTENCE_LENGTH];
unsigned long RTTYCounter=0;
unsigned long LoRaCounter=0;
unsigned long previousTX = 0;
volatile bool watchdogActivated = true;
volatile int sleepIterations = 0;


//============================================================================
void setup()
{
  // Setup Serial for debugging
  Serial.begin(9600);
  // Setup the Ublox GPS
  SerialGPS.begin(GPSBaud);  //TX, RX

  // Serial.println(F("Serial GPS"));

  // Setup the Radio
  ResetRadio(); 
  SetupRadio();  
  setup_PowerPins();
  enable_PowerPins();
}


//============================================================================
void loop()
{
   // Watchdog should have been fired before doing anything 
  if (watchdogActivated)
  {
     // REset the watrchdog and the sleep timer
     watchdogActivated = false;
     
     unsigned long currentMillis = millis();
   
     // Get data from the GPS
     CheckGPS(); 
     smartDelay(1000);
 
     // Process a non blocking timed loop
     if (currentMillis - previousTX >= ((unsigned long)TX_LOOP_TIME*(unsigned long)1000))
     {
       // Telemetry loop  
       previousTX = currentMillis;
     
       // Send RTTY
       if (RTTY_ENABLED)
       { 
          for (int i=1; i <= RTTY_REPEATS; i++)
          {
            CreateTXLine(RTTY_PAYLOAD_ID, RTTYCounter++, RTTY_PREFIX);
            sendRTTY(Sentence); 
          }
       }

       // Delay in milliseconds between rtty and lora. You can change this
       delay(1000);
     
       // Send LoRa 
       if (LORA_ENABLED)
       { 
          for (int i=1; i <= LORA_REPEATS; i++)
          {
            CreateTXLine(LORA_PAYLOAD_ID, LoRaCounter++, LORA_PREFIX);
            sendLoRa(Sentence); 
          }
       }
       // Goto to sleep after transmissions
       if (USE_DEEP_SLEEP && UGPS.Satellites > 3)
       {
         Serial.println("Going to sleep...");
         // Set all defined power pins to low
         disable_PowerPins();
         Serial.flush();
         sleepIterations = 0;    
         while (sleepIterations < TIME_TO_SLEEP)
         {
           sleep();
         }
        Serial.println("Awake!");
        // Set all defined power pins to high
        enable_PowerPins();
        previousTX = millis();
       }
    }
    watchdogActivated = true; 
  }
}
