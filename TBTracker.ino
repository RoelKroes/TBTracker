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
 * Version 0.2 - 2019-12-23 (Added external voltage support)
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

/***********************************************************************************
* PIN NUMBERS for SX1278
*  
* Change if needed
************************************************************************************/
#define PIN_NSS   10 
#define PIN_DIO0  2
#define PIN_BUSY  -1  // Not used in this sketch for sx1278
#define PIN_RESET -1  // Not used in this sketch for sx1278
#define PIN_DIO1  -1  // Not used in this sketch for sx1278

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
#define RTTY_PAYLOAD_ID  "RTTY-ID"    // Payload ID for RTTY protocol
#define RTTY_FREQUENCY  434.100      // Can be different from LoRa frequency
#define RTTY_SHIFT 600
#define RTTY_BAUD 75                 // Baud rate 100 and up is currently unstable. 75 Baud is recommended.
#define RTTY_STOPBITS 2
#define RTTY_PREFIX "$$$$$"           
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
#define LORA_PAYLOAD_ID  "LORA-ID"   // Payload ID for LoRa protocol
#define LORA_FREQUENCY  434.562      // Can be different from RTTY frequency
#define LORA_BANDWIDTH 125.0
#define LORA_SPREADFACTOR 9
#define LORA_CODERATE 7
#define LORA_PREFIX "$$"             // Some older LoRa software does not accept a prefix of more than 2x "$"
#define LORA_SYNCWORD 0x12           // for sx1278
// #define LORA_SYNCWORD 0x1424      // for sx1262 (currently not supported)
#define LORA_POWER 10                // in dBm between 2 and 17. 10 = 10mW
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
#define SENTENCE_LENGTH 100     // Maximum length of telemetry line to send

// Allow time for the GPS to re-acquire a fix when using sleep mode!
// Currently deep sleep is only enabled for ATMEGA328
// There is not a lot of effect as the LoRa and GPS chips consume a lot of power and these chps are currently not switched to power save mode
#define USE_DEEP_SLEEP false   // Put the ATMEGA328 chip to deep sleep while not transmitting. set to true or false.  
#define TIME_TO_SLEEP  15       // This is the number in seconds out of TX_LOOP_TIME that the CPU is in sleep. Only valid when USE_DEEP_SLEEP = true

#define TX_LOOP_TIME   30       // When USE_DEEP_SLEEP=false: Number in seconds between transmits
                                // When USE_DEEP_SLEEP=true : Time between transmits is TIME_TO_SLEEP+TX_LOOP_TIME+time it takes to transmit the data
                               
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
* SENSOR SETTINGS
*  
* Change if needed
* 
*  You can connect an external voltage directly to the EXTERNALVOLTAGE_PIN as long as the the pin is rated for that voltage
*  Alteratively, you can use a voltage divider so you can connect a higher voltage, but then you have to calculate the DIVIDER_RATIO yourself
*  
*  Voltage divider schema:
*  
*                          |-----------------
*                          |                |
*                          |               R1            
*                          |                |
*                    +/+ ---                |
*    to external voltage                    |------ To EXTERNALVOLTAGE_PIN
*                    -/- ---                |
*                          |                |
*                          |               R2  
*                          |                |
*                          |----------------------- To Arduino GND
*                          
*   DIVIDER_RATIO can be calculated by (R1+R2) / R2                       
************************************************************************************/
#define USE_EXTERNAL_VOLTAGE false // Set to true if you want to measure an external voltage on the EXTERNALVOLTAGE_PIN 
#define VCC_OFFSET 0.00            // Offset for error correction in Volts for the internal voltage. Ideally this should be 0.0 but usually is between -0.1 and +0.1 and is chip specific. 
#define EXT_OFFSET 0.00            // Offset for error correction in Volts for the external voltage. Use it to correct errors when necessary.
#define EXTERNALVOLTAGE_PIN A1     // Pin to read the external voltage from
#define SAMPLE_RES 1024            // 1024 for Uno, Mini, Nano, Mega, Micro. Leonardo. 4096 for Zero, Due and MKR  
#define DIVIDER_RATIO 1.00         // Leave at 1.00 when using no voltage divider. Set to (R1+R2)/R2 when using a voltage divider.

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
volatile unsigned long RTTYCounter=0;
volatile unsigned long LoRaCounter=0;
volatile unsigned long previousTX = 0;
volatile bool watchdogActivated = true;
volatile int sleepIterations = 0;


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
       if (USE_DEEP_SLEEP)
       {
         Serial.println("Going to sleep...");
         Serial.flush();
         sleepIterations = 0;    
         while (sleepIterations < TIME_TO_SLEEP)
         {
           sleep();
         }
        Serial.println("Awake!");
        previousTX = millis();
       }
    }
    watchdogActivated = true; 
  }
}
