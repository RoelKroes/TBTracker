#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define PEDESTRIAN 3
#define AIRBORNE 6 


// The TinyGPS++ object
TinyGPSPlus gps;

/*********************************************************************************************************************************/
void CheckGPS()
{    
  processGPSData();
  printGPSData();
}


/*********************************************************************************************************************************/
// This custom version of delay() ensures that the gps object is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (SerialGPS.available())
      gps.encode(SerialGPS.read());
  } while (millis() - start < ms);
}

/*********************************************************************************************************************************/
static void processGPSData()
{
  
  // Number of Satellites
  if (gps.satellites.isValid())
    UGPS.Satellites = gps.satellites.value();
  else
    UGPS.Satellites = 0;

 // Time
 if (gps.time.isValid())
 {
    UGPS.Hours = gps.time.hour();
    UGPS.Minutes = gps.time.minute();
    UGPS.Seconds = gps.time.second();
 }
 else
 {
    UGPS.Hours = 0;
    UGPS.Minutes = 0;
    UGPS.Seconds = 0;
 }

 // Position
 if (gps.location.isValid())
 {
    UGPS.Longitude = gps.location.lng();
    UGPS.Latitude = gps.location.lat();
 }
 else
 {
   UGPS.Longitude = 0;
   UGPS.Latitude = 0;
 }

 // Altitude
 if (gps.altitude.isValid())
    UGPS.Altitude = gps.altitude.meters();
 else
    UGPS.Altitude = 0;    

 if (UGPS.Altitude < 0)
   UGPS.Altitude = 0;    
   
}

/*********************************************************************************************************************************/
void printGPSData()
{
#if defined(DEVMODE)
  Serial.print(F("Time: ")); Serial.print(UGPS.Hours); Serial.print(":"); Serial.print(UGPS.Minutes); Serial.print(":"); Serial.println(UGPS.Seconds);
  Serial.print(F(" Lat: ")); Serial.println(UGPS.Latitude, 6);
  Serial.print(F(" Lon: ")); Serial.println(UGPS.Longitude, 6);
  Serial.print(F(" Alt: ")); Serial.println(UGPS.Altitude);
  Serial.print(F("Sats: ")); Serial.println(UGPS.Satellites);
  Serial.println("------");
#endif
}


/*********************************************************************************************************************************/
void SendUBX(unsigned char *Message, int Length)
{  
  int i;
  
  for (i=0; i<Length; i++)
  {
    SerialGPS.write(Message[i]);
  }
}
