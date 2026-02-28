
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define PIXEL_PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
uint16_t pixelSize = 50;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelSize, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,0,255);
uint32_t blue = strip.Color(0,255,0);
uint32_t magenta = strip.Color(255,255,0);
//uint32_t hotpink = strip.Color( 255,147,105 );
uint32_t hotpink = strip.Color( 255,55,51 );
//uint32_t yellow = strip.Color(255,0,255);
uint32_t yellow = strip.Color(255,0,155);

uint16_t currentPosition = pixelSize;

//SoftwareSerial voiceRecSerial(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.

byte voiceVal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //voiceRecSerial.begin(9600);
  initStrip();
  
  delay(2000);
  
  // Switch voice recognition to compact mode
  //voiceRec
  Serial.write(0xAA);
  //voiceRec
  Serial.write(0x37);
 
  delay(1000);

  // Import group 1 into the voice recognition module
  //voiceRec
  Serial.write(0xAA);
  //voiceRec
  Serial.write(0x21);

  Serial.println("Here we go");
  
}

void loop() {

  while ( Serial.available() && currentPosition == pixelSize) {
    voiceVal = Serial.read();
    currentPosition = 0; 
    switch (voiceVal) 
    {
        case 0x11:
          Serial.println("Blue was spoken!");
          break;
        case 0x12:
          Serial.println("Red was spoken!");
          break;
        case 0x13:
          Serial.println("Green was spoken!");
          break;
        case 0x14:
          Serial.println("Pink was spoken!");
          break;
        case 0x15:
          Serial.println("Yellow was spoken!");
          break;
    }
    
   
  }
  
  
  //if ( currentPosition < pixelSize ) {
  updateLEDStrip();
  //}
  
  delay(50);
 
}

void initStrip() {
  strip.begin();
  strip.setBrightness(255);
  for ( int i = 0; i < pixelSize; i++ ) {
    strip.setPixelColor(i,0,0,0);
  }
  strip.show();
}

void updateLEDStrip() {
  
  for ( int i = 0; i < pixelSize; i++ ) {
     if ( i == currentPosition ) {
       
       switch (voiceVal) 
       {
         case 0x11:
           strip.setPixelColor(currentPosition, blue);
           break;
         case 0x12:
           strip.setPixelColor(currentPosition, red);
           break;
         case 0x13:
           strip.setPixelColor(currentPosition, green);
           break;
         case 0x14:
           strip.setPixelColor(currentPosition, hotpink);
           break;
         case 0x15:
           strip.setPixelColor(currentPosition, yellow);
           break;
       }
    } else { 
      strip.setPixelColor(i,0,0,0);
    }
  }
  strip.show();
  currentPosition++;
  if ( currentPosition < pixelSize - 1 ) {
    currentPosition++;
  } else {
    currentPosition = pixelSize;
  }
  
}
