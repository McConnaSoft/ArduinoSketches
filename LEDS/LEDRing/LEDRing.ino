
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN 0
#define NUM_PIXELS 16
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 6, NEO_GRB + NEO_KHZ800);

// Rainbow Colors: http://colrd.com/palette/22198/?download=css

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);

uint32_t rb1 = strip.Color(255,51,17);
uint32_t rb2 = strip.Color(255,68,34);
uint32_t rb3 = strip.Color(255,102,68);
uint32_t rb4 = strip.Color(255,153,51);

uint32_t rb5 = strip.Color(254,174,45);
uint32_t rb6 = strip.Color(204,187,51);
uint32_t rb7 = strip.Color(208,195,16);
uint32_t rb8 = strip.Color(170,204,34);

uint32_t rb9 = strip.Color(105,208,37);
uint32_t rb10 = strip.Color(34,204,170);
uint32_t rb11 = strip.Color(18,189,185);
uint32_t rb12 = strip.Color(17,170,187);

uint32_t rb13 = strip.Color(68,68,221);
uint32_t rb14 = strip.Color(51,17,187);
uint32_t rb15 = strip.Color(59,12,189);
uint32_t rb16 = strip.Color(68,34,153);

uint32_t rainbow[16] = {rb1, rb2, rb3, rb4,
                        rb5, rb6, rb7, rb8,
                        rb9, rb10, rb11, rb12,
                        rb13, rb14, rb15, rb16};

int currentStartingPixel = 0;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  
  
  
  strip.begin();
  uint16_t n = strip.numPixels();
  //strip.setBrightness(64);
  for ( int i = 0; i < n; i++ ) {
    
    strip.setPixelColor(i,0,0,0);
  
  }
  strip.setBrightness(33);

  strip.show();
  
  //Serial.print("About to set red");
  
  //Serial.print("The number of pixels are: " ); Serial.println( n );

}

void loop() {
/*   
 // SIMPLE BLUE RING THAT SPINS THREE PIXELS WIDE   
  for ( int i = 0; i < NUM_PIXELS; i++ ) {
    if ( abs(currentPixel - i) <= 1 ) {
      strip.setPixelColor(i,blue);
    } else {
      strip.setPixelColor(i,0,0,0);
    }
  }
*/

  // SPINNING RAINBOW RING
  for ( int i=0; i < NUM_PIXELS; i++ ) {
    
    if ( i < currentStartingPixel ) {
      // If the LED is less than the current starting pixel we need to find the element that needs to be placed in the ith position
      // This is found by taken the total number of pixels and subtracting the difference between the current starting pixel and the ith position
      strip.setPixelColor(i, rainbow[NUM_PIXELS-(currentStartingPixel-i)]); 
    } else {
      strip.setPixelColor(i, rainbow[i-currentStartingPixel]);
    }
    
    //strip.setPixelColor(i, rainbow[i]);
  }

   
  strip.show();

  if ( currentStartingPixel < NUM_PIXELS-1 ) {
     currentStartingPixel++;
  } else {
     currentStartingPixel = 0;
  }
  
  delay(50); 
}
