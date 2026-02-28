#include <VirtualWire.h>

const char blueON = 'B';
const char blueOFF = 'b';
const char redON = 'R';
const char redOFF = 'r';

int rxPin = 11;

const int redLEDPin = 4;
const int blueLEDPin = 7;

void setup()
{
    Serial.begin(9600);
  
    pinMode(redLEDPin, OUTPUT);
    pinMode(blueLEDPin, OUTPUT);
    digitalWrite(redLEDPin, LOW);
    digitalWrite(blueLEDPin, LOW);
     
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_set_rx_pin(rxPin);
    vw_setup(4000);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
    
    Serial.println("Done setup");
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      
        Serial.println("Got message");
        Serial.println( buf[0] );
      
        if(buf[0]==blueON){
          digitalWrite(blueLEDPin,HIGH);
        }  
       if(buf[0]==blueOFF){
          digitalWrite(blueLEDPin,LOW);
        }
        if(buf[0]==redON){
          digitalWrite(redLEDPin,HIGH);
        }  
       if(buf[0]==redOFF){
          digitalWrite(redLEDPin,LOW);
        }
    }
}

