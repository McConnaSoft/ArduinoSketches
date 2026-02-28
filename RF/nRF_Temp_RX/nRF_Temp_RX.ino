/* YourDuinoStarter Example: nRF24L01 Receive Temperature values

 - WHAT IT DOES: Receives data from another transceiver with
   1 Analog value from a TMP36
   Displays received value on Serial Monitor
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
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char RecvPayload[31] = "";

void setup() 
{
  Serial.begin(9600);
  Serial.println("Nrf24L01 Receiver Starting");
  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}


void loop() 
{
  int len = 0;
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    while (!done)
    {
      // Fetch the data payload
      len = radio.getDynamicPayloadSize();
      done = radio.read( &RecvPayload, len );
      RecvPayload[len] = 0;
      Serial.print("Temp = ");
      Serial.println( RecvPayload );
      delay(5);
    }
  }
  
  RecvPayload[len] = 0;
  RecvPayload[0]=0;

}
