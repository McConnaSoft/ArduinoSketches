
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, 6, NEO_GRB + NEO_KHZ800);


uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,0,255);
uint32_t blue = strip.Color(0,255,0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  
  
  strip.begin();
  uint16_t n = strip.numPixels();
  //strip.setBrightness(64);
  //for ( int i = 0; i < n; i++ ) {
    
    //strip.setPixelColor(i,0,0,0);
  
  //}
  strip.setPixelColor(0, red);
  strip.setPixelColor(1, green); // strip.setPixelColor(0,255,0,0);
  strip.setPixelColor(2, blue); // strip.setPixelColor(0,255,0,0); 
  strip.setPixelColor(3, strip.Color(255,255,0));
  strip.setBrightness(66);
 // strip.setPixelColor(5, red);
 // strip.setPixelColor(3, green);
 // strip.setPixelColor(4, green);
  strip.show();
  
  //Serial.print("About to set red");
  
  Serial.print("The number of pixels are: " ); Serial.println( n );

}

void loop() {
  // put your main code here, to run repeatedly:
  //strip.setPixelColor(0,0,0,0);
  //strip.setPixelColor(1, red); // strip.setPixelColor(0,255,0,0);
  //strip.setPixelColor(2, red); // strip.setPixelColor(0,255,0,0); 
  //strip.setPixelColor(3, red);
  //for ( int i = 0; i < n; i++ ) {
  //  strip.setPixelColor(0,0,0,0);
  //}
   
  //strip.show();
 // delay(1000); 
}
