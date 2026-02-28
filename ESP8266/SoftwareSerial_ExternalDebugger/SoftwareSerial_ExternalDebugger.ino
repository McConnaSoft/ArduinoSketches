/*
 * McConnell
 * I used this sample to test external FTDI debugging.
 * Pin 10 - FTDI TX
 * Pin 11 - FTDI RX
 * I used the 3.3V FTDI Basic from SparkFun for this.
 * I use CoolTerm as a Serial Monitor.  Config file found in CoolTerm folder. 
 * So far 115200 is the best baud to use.
 */

#include "SoftwareSerial.h"


SoftwareSerial debugSerial(10, 11); // RX/TX on Arduino to TX/RX on FTDI



void setup(void)
{
    Serial.begin(115200); 
    
    debugSerial.begin( 115200 );
    
    debugSerial.println("Hello from software");
    Serial.println("Hello from hardware" );
    
}

void loop(void) {
  if (debugSerial.available()) {
    Serial.write(debugSerial.read());
  }
  if (Serial.available()) {
    debugSerial.write(Serial.read());  // This doesn't work when sent from Arduino Serial Monitor
  }
  
}
