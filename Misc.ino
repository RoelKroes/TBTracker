#include <EEPROM.h>

/***********************************************************************************
* The function CreateTXLine generates two payload sentences to transmit. One
* for RTTY and one for LoRa. These sentences are compatible with the habhub tracker.
* 
* Run the software and decode your payload sentences. Then go to 
* http://habitat.habhub.org/genpayload/ and register your payload there by
* generating a payload configuration document. You will need the generated sentences
* to create a payload configuration document. When you do that, other people will be
* able to see your tracker on the map at https://tracker.habhub.org
*
* Sample of a RTTY payload sentence this software will generate:
* 
* $$RTTY-ID,5,12:11:10,53.16307,6.16444,12,3,12.0,4.61,3.71*109F 
* 
* This sentence contains the following fields, separated by comma's:
*     $$        = prefix
*     RTTY-ID   = callsign
*     5         = sequence number
*     12:11:10  = Time (UTC)
*     53.163067 = Latitude
*     6.16444   = Longitude
*     12        = Altitude
*     3         = Number of satellites
*     12.0      = Internal (chip) temperature in Celsius
*     4.61      = VCC voltage
*     3.71      = External voltage on analog pin A1
*     *         = seprator
*     109F      = Checksum
*     
* The LoRa payload looks the same, except for the callsign (if you changed that).     
************************************************************************************/
//===============================================================================
void CreateTXLine(const char *PayloadID, unsigned long aCounter, const char *aPrefix)
{
   char Sen[5];
   int Count, i, j;
   unsigned char c;
   unsigned int CRC, xPolynomial;
   char LatitudeString[16], LongitudeString[16], CRCString[8];
   char InternalTemp[10];
   char BattVoltage[10];
   char ExtVoltage[10];

   // Get the internal chip temperature
   dtostrf(ReadTemp(), 3, 1, InternalTemp);

   // Get the battery voltage
   dtostrf(ReadVCC(), 4, 2,BattVoltage);
         
   dtostrf(UGPS.Latitude, 7, 5, LatitudeString);
   dtostrf(UGPS.Longitude, 7, 5, LongitudeString);   
   
   sprintf(Sentence,
#if defined(USE_FIELDSTR)               
            "%s%s,%ld,%02d:%02d:%02d,%s,%s,%ld,%u,%u,%s,%s,%s"
#else
            "%s%s,%ld,%02d:%02d:%02d,%s,%s,%ld,%u,%u,%s,%s"
#endif
            ,
            aPrefix,
            PayloadID,
            aCounter,
            UGPS.Hours, UGPS.Minutes, UGPS.Seconds,   
            LatitudeString,
            LongitudeString,
            UGPS.Altitude,
            UGPS.Satellites,
            UGPS.Heading,
            InternalTemp,
            BattVoltage
#if defined(USE_FIELDSTR)            
            ,
            FIELDSTR
#endif            
            );

   Count = strlen(Sentence);

   // Calc CRC
   CRC = 0xffff;           // Seed
   xPolynomial = 0x1021;
   
   for (i = strlen(aPrefix); i < Count; i++)
   {   // For speed, repeat calculation instead of looping for each bit
      CRC ^= (((unsigned int)Sentence[i]) << 8);
      for (j=0; j<8; j++)
      {
          if (CRC & 0x8000)
              CRC = (CRC << 1) ^ 0x1021;
          else
              CRC <<= 1;
      }
   }

   Sentence[Count++] = '*';
   Sentence[Count++] = Hex((CRC >> 12) & 15);
   Sentence[Count++] = Hex((CRC >> 8) & 15);
   Sentence[Count++] = Hex((CRC >> 4) & 15);
   Sentence[Count++] = Hex(CRC & 15);
   Sentence[Count++] = '\n';  
   Sentence[Count++] = '\0';
}

//===============================================================================
char Hex(char Character)
{
  char HexTable[] = "0123456789ABCDEF";
  
  return HexTable[Character];
}


//===============================================================================
//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

//===============================================================================
//This function will take and assembly 4 byte of the Eeprom memory in order to form a long variable.
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }

//===============================================================================
//This function will reset the transmission counters to 0.
void Reset_Transmission_Counters()
{
   EEPROMWritelong(0x00,0);
   EEPROMWritelong(0x04,0);
}
