/*
 Demonstration sketch for Adafruit i2c/SPI LCD backpack
 using MCP23008 I2C expander
 ( http://www.ladyada.net/products/i2cspilcdbackpack/index.html )

 This sketch prints "Hello World!" to the LCD
 and shows the time since the board was reset.

  The circuit see: http://www.imaginen4tion.blogspot.com/2015/10/how-to-connect-esp8266-to-lcd-via-i2c.html

*/

// Originally written by Adafruit.
// Modified for esp8266 by Bob Rowe on 10.31.15, giddyup!

// include the library code:
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
// Note, problems with your libraries is a common issue due to multiple
// libraries with the same name getting imported into your Arduino IDE
// when setting up to develop on esp8266.
// See for more info:


// Connect to LCD via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);
// "Default address" simply refers to how your Adafruit i2c backpack is setup.
// See here for more info:  https://learn.adafruit.com/i2c-spi-lcd-backpack/connect-to-i2c

void setup() { 
  // Initiate the i2c connection on esp pins 2 (DAT/SDA) and 14 (CLK/SCL)
  Wire.begin(2,14);

  // set up the LCD's number of rows and columns:
  // Here we are using a standard LCD with 16 columns and 2 rows
  // example of LCD:  https://www.adafruit.com/products/181
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  /*
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  */
}
