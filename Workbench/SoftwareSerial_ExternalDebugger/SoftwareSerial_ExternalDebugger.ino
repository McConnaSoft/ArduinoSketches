#include "SoftwareSerial.h"


SoftwareSerial debugSerial(10, 11); 


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
