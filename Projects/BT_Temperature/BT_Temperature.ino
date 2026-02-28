/*********************************************************************
This is an example for our nRF8001 Bluetooth Low Energy Breakout

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1697

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Kevin Townsend/KTOWN  for Adafruit Industries.
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

// This version uses call-backs on the event and RX so there's no data handling in the main loop!

#include <SPI.h>
#include "Adafruit_BLE_UART.h"
#include <Wire.h>
#include <math.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
bool isConnected = 0;
bool hasSent = 0;
float prevTemp = -200.0;
unsigned long prevTempCheck = 0;
unsigned long prevTempTX = 0;

/**************************************************************************/
/*!
    This function is called whenever select ACI events happen
*/
/**************************************************************************/
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
    case ACI_EVT_DEVICE_STARTED:
      Serial.println(F("Advertising started"));
      break;
    case ACI_EVT_CONNECTED:
      Serial.println(F("Connected!"));
      isConnected = 1;
      break;
    case ACI_EVT_DISCONNECTED:
      Serial.println(F("Disconnected or advertising timed out"));
      isConnected = 0;
      break;
    default:
      break;
  }
}

/**************************************************************************/
/*!
    This function is called whenever data arrives on the RX channel
*/
/**************************************************************************/
void rxCallback(uint8_t *buffer, uint8_t len)
{
  Serial.print(F("Received "));
  Serial.print(len);
  Serial.print(F(" bytes: "));
  for(int i=0; i<len; i++)
   Serial.print((char)buffer[i]); 

  Serial.print(F(" ["));

  for(int i=0; i<len; i++)
  {
    Serial.print(" 0x"); Serial.print((char)buffer[i], HEX); 
  }
  Serial.println(F(" ]"));

  ///* Echo the same data back! */
  //uart.write(buffer, len);
}

/**************************************************************************/
/*!
    Configure the Arduino and start advertising with the radio
*/
/**************************************************************************/
void setup(void)
{ 
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
  //Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Callback Echo demo"));
  
  prevTempCheck = millis();
  
  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  // uart.setDeviceName("NEWNAME"); /* 7 characters max! */
  uart.begin();
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
}

/**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/
void loop()
{
  uart.pollACI();
  //if ( isConnected && !hasSent ) {
  //  sendTestMessage();   
  //  hasSent = 1;
  //}
  
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);
  float temperature;
  bmp.getTemperature(&temperature);
  Serial.println(temperature);
  
  unsigned long currentTime = millis();
 
  if ( currentTime - prevTempCheck > 250 ) 
  {
     /* Get a new sensor event */ 
    //sensors_event_t event;
    //bmp.getEvent(&event);
    
    /* Display the results (barometric pressure is measure in hPa) */
    if (event.pressure)
    {
      //float temperature;
      //bmp.getTemperature(&temperature);
      
      //if ( floor(prevTemp) != floor(temperature) ) { 
      // Conditions:
      // 1) Temperature has not been recorded yet
      // 2) Time since last recording is past five minutes
      // 3) Temperature has increased more than 0.4 degrees since last loop  
      if ( prevTempTX == 0 || abs(currentTime - prevTempTX) > (unsigned long)5*60*1000 || abs(prevTemp - temperature) > 0.4 ) {   
        //Serial.println( abs(currentTime - prevTempTX) );
        
        if ( isConnected ) {
      
          /* Display atmospheric pressure in hPa */
          Serial.print("Pressure: "); Serial.print(event.pressure); Serial.println(" hPa");
        
          /* First we get the current temperature from the BMP085 */
          Serial.print("Temperature: ");
          Serial.print(temperature);
          Serial.println(" C");
      
          
          String message;
          char temp[10];
          
          String temperatureString;
          dtostrf(temperature,1,2,temp);
          temperatureString = String(temp);
          
          String pressureString;
          dtostrf(event.pressure,1,2,temp);
          pressureString = String(temp);
          
          message = "T:" + temperatureString + "," + "P:" + pressureString;
   
          sendBTLEMessage( message );
          prevTempTX = millis();
        
      }
        
        prevTemp = temperature;
      }
      
    }
    else
    {
      Serial.println("Sensor error");
    }
    prevTempCheck = currentTime;
  } 
  delay(200);
}

void sendBTLEMessage(String message) 
{   
      //String s = "From Arduino";
      // We need to convert the line to bytes, no more than 20 at this time
      uint8_t sendbuffer[20];
      message.getBytes(sendbuffer, 20);
      char sendbuffersize = min(20, message.length());
      Serial.print(F("\n* Sending -> \"")); Serial.print((char *)sendbuffer); Serial.println("\"");
      // write the data
      uart.write(sendbuffer, sendbuffersize);
}

