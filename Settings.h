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
#define FSK_FREQUENCY 434.126
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
* Default RTTY setting is: 7,N,2 at 100 Baud.
************************************************************************************/
#define RTTY_ENABLED false            // Set to true if you want RTTY transmissions (You can use Both LoRa and RTTY or only one of the two) 
#define RTTY_PAYLOAD_ID  "RTTY_ID"    // Payload ID for RTTY protocol
#define RTTY_FREQUENCY  434.126      // Can be different from LoRa frequency
#define RTTY_SHIFT 610
#define RTTY_BAUD 100                // Baud rate
#define RTTY_STOPBITS 2
#define RTTY_PREFIX "$$$$$$"          
 
// RTTY encoding modes (leave this unchanged)
#define RTTY_ASCII 0                 // 7 data bits 
#define RTTY_ASCII_EXTENDED 1        // 8 data bits
#define RTTY_ITA2  2                 // Baudot 


#define RTTY_REPEATS 1 // number of RTTY transmits during a cycle

// Idle carrier in ms before sending actual RTTY string. 
// Set to a low value (i.e. 1000 or lower) if you have a very frequency stable signal
// Set to a high value (i.e. 5000 or even higher) if you have a hard time to tune the signal
#define RTTY_IDLE_TIME 4000          
 
/***********************************************************************************
* LORA SETTINGS
*  
* Change when needed
************************************************************************************/
#define LORA_ENABLED true            // Set to true if you want LoRa transmissions (You can use Both LoRa and RTTY or only one of the two)
#define LORA_PAYLOAD_ID  "LORA_ID"   // Payload ID for LoRa protocol
#define LORA_FREQUENCY  434.126      // Can be different from RTTY frequency
#define LORA_BANDWIDTH 125.0         // Do not change, change LORA_MODE instead
#define LORA_SPREADFACTOR 9          // Do not change, change LORA_MODE instead
#define LORA_CODERATE 7              // Do not change, change LORA_MODE instead
#define LORA_PREFIX "$$"             // Prefix for "Telemetry". Some older LoRa software does not accept a prefix of more than 2x "$"
#define LORA_SYNCWORD 0x12           // Default syncword
#define LORA_POWER 10                // in dBm between 2 and 17. 10 = 10mW (recommended)
#define LORA_CURRENTLIMIT 100
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0
// HAB modes
// 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on  - SUPPORTED
// 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off - SUPPORTED
// 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off - SUPPORTED
// 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off - SUPPORTED
// 4 = Test mode not for normal use. - NOT SUPPORTED
// 5 = (normal for calling mode)   Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off - SUPPORTED
#define LORA_MODE 2  // Mode 2 is usually used for simple telemetry data
#define LORA_REPEATS 1 // number of LoRa transmits during a cycle

/***********************************************************************************
* TRANSMISSIONS SETTINGS
*  
* Change if needed
************************************************************************************/
#define SENTENCE_LENGTH 100     // Maximum length of telemetry line to send

// Allow time for the GPS to re-acquire a fix when using sleep mode!
// Currently deep sleep is only enabled for ATMEGA328
#define USE_DEEP_SLEEP false    // Put the ATMEGA328 chip to deep sleep while not transmitting. set to true or false.
                                // The tracker will only go to sleep if there are more than 4 satellites visible   
#define TIME_TO_SLEEP  15       // This is the number in seconds out of TX_LOOP_TIME that the CPU is in sleep. Only valid when USE_DEEP_SLEEP = true

#define TX_LOOP_TIME   45       // When USE_DEEP_SLEEP=false: Number in seconds between transmits
                                // When USE_DEEP_SLEEP=true : Time between transmits is TIME_TO_SLEEP+TX_LOOP_TIME+time it takes to transmit the data

#define DEVMODE // Development mode. Uncomment to enable for debugging and see debug info on the serial monitor
                // Comment this out if you experience out-of-memory errors.
                              
/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
// GPS Serial device
// We use SoftwareSerial so these pin numbers are basically free to choose
// Parameters for the GPS
// White: 7, green: 8
static const int Rx = 8, Tx = 7;
static const uint32_t GPSBaud = 9600;

/***********************************************************************************
* SONDEHUB EXTRA FIELDS SETTINGS
*  
* For displaying extra fields at sondehub, we need to define which fields are
* in the telemetry after the lat, lon, alt fields
* This can be done by adding a specific string after the last telemetry field
* This is supported by the various receivers made by Dave Akerman,
* See: https://www.daveakerman.com/?page_id=2410
* 
* 0  PayloadID
* 1 Counter
* 2 Time
* 3 Latitude
* 4 Longitude
* 5 Altitude
* 6 Satellites
* 7 Speed
* 8 Heading
* 9 Battery Voltage
* A InternalTemperature
* B ExternalTemperature
* C PredictedLatitude
* D PredictedLongitude
* E CutdownStatus
* F LastPacketSNR
* G LastPacketRSSI
* H ReceivedCommandCount
* I-N ExtraFields
* O MaximumAltitude
* P Battery Current
* Q External Temperature 2
* R Pressure
* S Humidity
* T CDA
* U Predicted Landing Speed
* V Time Till Landing
* W Last Command Received
* 
* Our string would be: "01234568A9"
* You can disable FIELDSTR by undefining it, if you want.
************************************************************************************/
#define USE_FIELDSTR
#define FIELDSTR "01234568A9"


/***********************************************************************************
* TELEMETRY COUNTERS
*  
* Uncomment this if you want to reset the counters for LoRa and RTTY set back to 0.
************************************************************************************/
// #define RESET_TRANS_COUNTERS 
