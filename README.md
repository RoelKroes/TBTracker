# TBTracker
RTTY and LoRa High Altitude Balloon Tracker for Arduino and SX1278

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

You will need two extra Arduino libraries to be able to compile this sketch.:

 https://github.com/jgromes/RadioLib (Radiolib)
 https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)
 
 
 To run this, you need a TTGO T-Deer board or:
 *  1 x Arduino Mini - 3.3v (or compatible Arduino board)
 *  1 x SX1278 LoRa chip (or compatible LoRa chip. Basically it will work with all sx127x series chips)
 *  1 x BN220 GPS (9600 Baud) or any UBlox or compatible GPS device
 
 Many parameters are user adjustable.
 
 Have fun,
 
 Roel.
