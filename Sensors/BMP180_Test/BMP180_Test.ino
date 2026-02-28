#include <Wire.h>
#include <math.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
   
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

float prevTemp = -200.0;
 
void setup(void) 
{
  Serial.begin(9600);
  Serial.println(""); Serial.println("Pressure Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
}
 
void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);
 
  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    float temperature;
    bmp.getTemperature(&temperature);
    
    if ( floor(prevTemp) != floor(temperature) ) { 
      /* Display atmospheric pressure in hPa */
      Serial.print("Pressure: "); Serial.print(event.pressure); Serial.println(" hPa");
    
      /* First we get the current temperature from the BMP085 */
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");
    }
    prevTemp = floor(temperature);
  }
  else
  {
    Serial.println("Sensor error");
  }
  delay(250);
}
