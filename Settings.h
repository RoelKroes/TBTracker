/************************************************************************
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
#define FSK_FREQUENCY 434.113
#define FSK_BITRATE 100.0
#define FSK_FREQDEV 50.0
#define FSK_RXBANDWIDTH 125.0
#define FSK_POWER 10   // in dBm between 2 and 17. 10 = 10mW (recommended). Sets also RTTY power
#define FSK_PREAMBLELENGTH 16
#define FSK_ENABLEOOK false
#define FSK_DATASHAPING 0.5

/***********************************************************************************
* RTTY SETTINGS
*  
* Change when needed
************************************************************************************/
#define RTTY_ENABLED true            // Set to true if you want RTTY transmissions (You can use Both LoRa and RTTY or only one of the two) 
#define RTTY_PAYLOAD_ID  "RTTY_ID"   // Payload ID for RTTY protocol
#define RTTY_FREQUENCY  434.113      // Can be different from LoRa frequency
#define RTTY_SHIFT 610
#define RTTY_BAUD 150                // Baud rate
#define RTTY_STOPBITS 2
#define RTTY_PREFIX "$$$$$"          
 
// RTTY encoding modes (leave this unchanged)
#define RTTY_ASCII 0                 // 7 data bits 
#define RTTY_ASCII_EXTENDED 1        // 8 data bits
#define RTTY_ITA2  2                 // Baudot 


#define RTTY_REPEATS 1 // number of RTTY transmits during a cycle

// Idle carrier in ms before sending actual RTTY string. 
// Set to a low value (i.e. 1000 or lower) if you have a very frequency stable signal
// Set to a high value (i.e. 5000 or even higher) if you have a hard time to tune the signal
#define RTTY_IDLE_TIME 2500          
 
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
#define LORA_POWER 10                // in dBm between 2 and 17. 10 = 10mW (recommended)
#define LORA_CURRENTLIMIT 100
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0
// HAB modes
// 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on  - NUT SUPPORTED YET
// 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off  - NUT SUPPORTED YET
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
#define USE_DEEP_SLEEP true     // Put the ATMEGA328 chip to deep sleep while not transmitting. set to true or false.
                                // The tracker will only go to sleep if there are more than 4 satellites visible   
#define TIME_TO_SLEEP  15       // This is the number in seconds out of TX_LOOP_TIME that the CPU is in sleep. Only valid when USE_DEEP_SLEEP = true

#define TX_LOOP_TIME   30       // When USE_DEEP_SLEEP=false: Number in seconds between transmits
                                // When USE_DEEP_SLEEP=true : Time between transmits is TIME_TO_SLEEP+TX_LOOP_TIME+time it takes to transmit the data

// Define up to 5 pins to power sensors from (for example your GPS). Each Arduino pin can source up to 40mA. All together, the pins can source 150-200 mA
// Use a transistor or FET as a switch if you need more power. Or use multiple pins in parallel.
// This will only work when USE_DEEP_SLEEP=true and there is a valid GPS lock.
// Comment out the pins you use for your sensors or leds. 
// Set pin value to a valid value.
#define POWER_PIN1     3
#define POWER_PIN2     4
// #define POWER_PIN3     -1
// #define POWER_PIN4     -1
// #define POWER_PIN5     -1


#define DEVMODE // Development mode. Uncomment to enable for debugging and see debug info on the serial monitor
                              
/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
// GPS Serial device
// We use SoftwareSerial so these pin numbers are basically free to choose
// Parameters for the GPS
// White: 7, green: 8
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
*   
*   Optionally add a 100nF capacitor between A1 and GND if the measured voltage seems unstable
************************************************************************************/
#define USE_EXTERNAL_VOLTAGE false // Set to true if you want to measure an external voltage on the EXTERNALVOLTAGE_PIN 
#define VCC_OFFSET 0.00            // Offset for error correction in Volts for the internal voltage. Ideally this should be 0.0 but usually is between -0.1 and +0.1 and is chip specific. 
#define EXT_OFFSET 0.00            // Offset for error correction in Volts for the external voltage. Use it to correct errors when necessary.
#define EXTERNALVOLTAGE_PIN A1     // Pin to read the external voltage from
#define SAMPLE_RES 1024            // 1024 for Uno, Mini, Nano, Mega, Micro. Leonardo. 4096 for Zero, Due and MKR  
#define DIVIDER_RATIO 1.00         // Leave at 1.00 when using no voltage divider. Set to (R1+R2)/R2 when using a voltage divider.

/***********************************************************************************
* TELEMETRY COUNTERS
*  
* Uncomment this if you want to reset the counters for LoRa and RTTY set back to 0.
************************************************************************************/
// #define RESET_TRANS_COUNTERS 
