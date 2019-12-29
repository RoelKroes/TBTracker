/***********************************************************************************
* The function CreateTXLine generates two payload sentences to transmit. One
* for RTTY and one for LoRa. These sentences are compatible with the habhub tracker.
* 
* Rrun the software and decode your payload sentences. Then go to 
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
* The L<oRa payload looks the same, except for the callsign (if you changed that).     
************************************************************************************/


//===============================================================================
void CreateTXLine(const char *PayloadID, unsigned long aCounter, const char *aPrefix)
{
   /*
    *  This function needs to be optimized in a major way.
    */
    
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

   // Get the external voltage
   dtostrf(ReadExternalVoltage(), 4, 2,ExtVoltage);
         
   dtostrf(UGPS.Latitude, 7, 5, LatitudeString);
   dtostrf(UGPS.Longitude, 7, 5, LongitudeString);   
   
   sprintf(Sentence,
            "%s%s,%ld,%02d:%02d:%02d,%s,%s,%ld,%u,%s,%s,%s",
            aPrefix,
            PayloadID,
            aCounter,
            UGPS.Hours, UGPS.Minutes, UGPS.Seconds,   
            LatitudeString,
            LongitudeString,
            UGPS.Altitude,
            UGPS.Satellites,
            InternalTemp,
            BattVoltage,
            ExtVoltage);

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
