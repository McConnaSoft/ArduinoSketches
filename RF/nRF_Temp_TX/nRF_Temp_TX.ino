/* YourDuinoStarter Example: nRF24L01 Transmit Temperature
 - WHAT IT DOES: Reads Analog values on A0 and transmits
   them over a nRF24L01 Radio Link to another transceiver.
 - SEE the comments after "//" on each line below
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
   - 
   TMP36:
   GND to Arduino GND
   VCC to Arduino +5V
   Tmp36 Out to Arduino A0
*/   
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
#define TEMP36_PIN A0

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

void setup()
{
  
  Serial.begin(9600);
  Serial.println("Beggining TX...");
  radio.begin();
  radio.openWritingPipe(pipe);
}


void loop() 
{
  int sensorValue = analogRead(TEMP36_PIN);
  float voltage = (sensorValue/1024.0) * 5.0;
  float temp = (voltage - .5) * 100;
  char tempBuf[10];
  char finalBuf[10];
  
  dtostrf(temp, 6, 2, tempBuf); 
  sprintf(finalBuf,"%s",tempBuf);
  radio.write( finalBuf, sizeof(finalBuf) );
  
  delay(500);
}
