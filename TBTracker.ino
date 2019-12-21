#include <SoftwareSerial.h>

/***********************************************************************************
 * LoRa and RTTY tracker for Arduino Mini and SX1278
 * Specially designed for the cheap TTGO T-deer board
 * a TTGO T-Deer board is an Arduino Mini at 3.3v with a SX1278 LoRa chip on board
 * 
 * by Roel Kroes
 * roel@kroes.com
 * 
 * Version 0.1 - 2019-12-19
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
 ************************************************************************************/

/***********************************************************************************
* PIN NUMBERS
*  
* Change if needed
************************************************************************************/
#define PIN_NSS   10 
#define PIN_DIO0  2
#define PIN_BUSY  9
#define PIN_RESET 7
#define PIN_DIO1  -1  // Not used in this sketch

/***********************************************************************************
* DEFAULT FSK SETTINGS
*  
* Normally needs no change
************************************************************************************/
#define FSK_FREQUENCY 434.100
#define FSK_BITRATE 100.0
#define FSK_FREQDEV 50.0
#define FSK_RXBANDWIDTH 125.0
#define FSK_POWER 10   // in dBm between 2 and 17. 10 = 10mW
#define FSK_CURRENTLIMIT 100
#define FSK_PREAMBLELENGTH 16
#define FSK_ENABLEOOK false
#define FSK_DATASHAPING 0.5

/***********************************************************************************
* RTTY SETTINGS
*  
* Change when needed
************************************************************************************/
#define RTTY_ENABLED true            // Set to true if you want RTTY transmissions (You can use Both LoRa and RTTY or only one of the two) 
#define RTTY_PAYLOAD_ID  "RTTY_ID"    // Payload ID for RTTY protocol
#define RTTY_FREQUENCY  434.100      // Can be different from LoRa frequency
#define RTTY_SHIFT 600
#define RTTY_BAUD 75                 // Baud rate 100 and up is currently unstable. 75 Baud is recommended.
#define RTTY_STOPBITS 2
#define RTTY_PREFIX "$$$$"           
// RTTY encoding modes (leave this unchanged)
#define RTTY_ASCII 0                 // 7 data bits 
#define RTTY_ASCII_EXTENDED 1        // 8 data bits
#define RTTY_ITA2  2                 // Baudot 
#define RTTY_REPEATS 1 // number of RTTY transmits during a cycle

/***********************************************************************************
* LORA SETTINGS
*  
* Change when needed
************************************************************************************/
#define LORA_ENABLED true            // Set to true if you want LoRa transmissions (You can use Both LoRa and RTTY or only one of the two)
#define LORA_PAYLOAD_ID  "LORA_ID"   // Payload ID for LoRa protocol
#define LORA_FREQUENCY  434.562      // Can be different from RTTY frequency
#define LORA_BANDWIDTH 125.0
#define LORA_SPREADFACTOR 9
#define LORA_CODERATE 7
#define LORA_PREFIX "$$"             // Some older LoRa software does not accept a prefix of more than 2x "$"
#define LORA_SYNCWORD 0x12           // for sx1278
// #define LORA_SYNCWORD 0x1424      // for sx1262 (currently not supported)
#define LORA_POWER 10  
#define LORA_CURRENTLIMIT 100
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0
// HAB modes
// 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on
// 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off
// 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off
// 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off
// 4 = Test mode not for normal use.
// 5 = (normal for calling mode)   Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off
// Default UKHAS tracker mode only 0,1,2 and 3 are implemented in this code
#define LORA_MODE 2  // Mode 2 is usuually used for simple telemetry data
#define LORA_REPEATS 1 // number of LoRa transmits during a cycle

/***********************************************************************************
* TRANSMISSIONS SETTINGS
*  
* Change if needed
************************************************************************************/
// When USE_DEEP_SLEEP = true, the time between transmissions will be TX_LOOP_TIME + TIME_TO_SLEEP*8000 (in milli seconds)
// When USE_DEEP_SLEEP = false, the time between transmissions will be TX_LOOP_TIME (in milli seconds)
// Allow time for the GPS to re-acquire a fix when using sleep mode!
// Currently deep sleep is only enabled for Arduino
#define USE_DEEP_SLEEP  false   // Put the chip to deep sleep while not transmitting true or false
#define TIME_TO_SLEEP  2        // This number is in 8 seconds interval (so a value of 3 is 3x8=24 seconds sleep time
#define TX_LOOP_TIME 30000      // in millis
#define SENTENCE_LENGTH 100     // Maximum length of telemetry line to send

/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
// GPS Serial device
// We use SoftwareSerial so these pin numbers are basically free to choose
// Parameters for the GPS
static const int Rx = 7, Tx = 8;
static const uint32_t GPSBaud = 9600;


/***********************************************************************************
* DATA STRUCTS
*  
* Normally no change necessary
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


//============================================================================
void setup()
{
  // Setup Serial for debugging
  Serial.begin(9600);
  // Setup the Ublox GPS
  SerialGPS.begin(GPSBaud);  //TX, RX
  Serial.println(F("Serial GPS"));

  // Setup the Radio
  ResetRadio(); 
  SetupRadio();  
}


//============================================================================
void loop()
{
  unsigned long currentMillis = millis();
   
  // Get data from the GPS
  CheckGPS(); 
  smartDelay(1000);
 
  // Process a non blocking timed loop
  if (currentMillis - previousTX >= TX_LOOP_TIME)
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

     // Delay 1 sec between rtty and lora
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

     // Put the CPU to sleep
     if (USE_DEEP_SLEEP)
     {
       SleepNow(); // Goodnight
     }
     UGPS.Satellites = 0;
  }
}
