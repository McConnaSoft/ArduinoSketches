#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <SoftwareSerial.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

#define WIFI
#define WIFI_BAUD 115200

#ifdef WIFI
    // Using SoftwareSerial for debugging requires an FTDI breakout to send the serial communications to putty or a serial monitor
    SoftwareSerial wifiSerial(10,11);
    
    #define WIFILOG(message)     wifiSerial.println(message)
    #define WIFILOGLN(message)   wifiSerial.println(message)
    #define WIFIBEGIN()          wifiSerial.begin(115200)
    #define WIFILOADWAIT()       delay(10000)
#else
    #define WIFILOG(message)
    #define WIFILOGLN(message)
    #define WIFIBEGIN()   
    #define WIFILOADWAIT()
#endif


void setup() {
  Serial.begin(9600);
  WIFIBEGIN();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if(!bmp.begin())
  {
    // There was a problem detecting the BMP085 ... check your connections 
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  //delay(1000); // Pause for 1 second

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Workbench 1.0"));
  display.println(F(""));
  display.println(F("Loading....."));
  display.display();

  WIFILOADWAIT();
  
}

void loop() {
    float temperature;
    bmp.getTemperature(&temperature);
    writeTemperature( temperature );
    
    char temp[10];
    dtostrf(temperature,1,2,temp);
    String tempString = String(temp);
    String postDataString = "temp=" + tempString;
    
    WIFILOGLN(postDataString);
    
    delay(1000);
}

void writeTemperature( float temp) {
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0,0);  

    display.print("Temperature: ");
    display.print(temp);
    display.println(" C");
    display.display();
}

  
