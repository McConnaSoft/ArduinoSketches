
#include <Adafruit_NeoPixel.h>

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
uint32_t hotpink = strip.Color( 255,147,105 );

uint16_t currentPosition = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  
  strip.begin();
  strip.setBrightness(255);
  for ( int i = 0; i < pixelSize; i++ ) {
    strip.setPixelColor(i,0,0,0);
  }
  strip.show();
  
  uint16_t n = strip.numPixels();
  Serial.print("The number of pixels are: " ); Serial.println( n );

}

void loop() {
 
 for ( int i = 0; i < pixelSize; i++ ) {
   if ( i == currentPosition ) {
       strip.setPixelColor(currentPosition, red);
       if ( i - 1 >= 0 ) {
         strip.setPixelColor(i-1, blue);
       }
       if ( i - 2 >= 0 ) {
         strip.setPixelColor(i-2, green); // green
       }
       if ( i - 3 >= 0 ) {
         strip.setPixelColor(i-3, hotpink); // magenta
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
   currentPosition = 0;
 }
 delay(50);
}
