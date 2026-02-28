/* McConnell
 *  This is used to test basic AT commands using the ESP8266 as a module on the arduino.
 *  Debugging is performed using SoftwareSerial, an FTDI breakout, and CoolTerm (on Mac)
 *  Baud of both modules is 115200
 *  The ESP8266 has to be wired to RX = 0 and TX = 1 on the Arduino
 *  The ESP8266 is in an ESP-01 Adapter module that does 5-3.3V voltage conversion
 *  The FTDI used was the FTDI Basic from SparkFun
 *  Serial Monitor for debugging is done in CoolTerm -> CoolTerm_ArduinoDebugging.stc
 *  
 *  Warnings:  TX/RX pins need to be empty when programming Arduino
 */

#include <SoftwareSerial.h>

SoftwareSerial debugSerial(10, 11);   // Pin 10 goes into FTDI TX, Pin 11 into RX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  debugSerial.begin( 115200 );
  debugSerial.println("Started");

  // set the data rate for the SoftwareSerial port
  debugSerial.println("Resetting ESP8266...");
  debugSerial.println("Going to try an AT call now...");
  Serial.write("AT+RST\r\n");
  
 
}

void loop() {
  
  while (Serial.available()) {
    debugSerial.write(Serial.read());
  }
  while (debugSerial.available()) {
    Serial.write(debugSerial.read());
  }  
}
