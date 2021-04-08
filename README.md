# TBTracker
RTTY and LoRa High Altitude Balloon Tracker for Arduino and SX127x (or Hope RFM9x).

TBTracker is a sketch for a tracker for high altitude weather balloons. It will send telemetry data in RTTY and LoRa format.
It is designed to send telemetry data in the correct format for https://tracker.habhub.org/

It can send data in RTTY or LoRa format on different frequencies and with different callsigns.

Currently it supports:
- Time
- Longitude
- Latitude
- Altitude
- Number of satellites visible
- Internal temperature
- Internal voltage
- External voltage

The code as is, is for the SX1278 module. If you have a SX1276, then change line 5 in radio.ino. See the comments in that file.

You will need two extra Arduino libraries to be able to compile this sketch.:

 https://github.com/jgromes/RadioLib (Radiolib)
 
 https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)
 
 
 To run this, you need a TTGO T-Deer board or:
 *  1 x Arduino Mini - 3.3v (or compatible Arduino board)
 *  1 x SX1278 LoRa chip (or compatible LoRa chip. Basically it will work with all sx127x series chips)
 *  1 x ATGM336H, BN220 GPS (9600 Baud) or any UBlox or compatible GPS device like a NEO6 or NEO8. I recommend the ATGM336H GPS module.
 
 Connect the T-Deer as an Arduino Mini to the Arduino IDE.
 Many parameters are user adjustable in settings.h
 
 Have fun,
 
 Roel.
