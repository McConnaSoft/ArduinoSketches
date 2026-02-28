
#include <Adafruit_NeoPixel.h>

#define LED_DATA_PIN 5
#define NUM_PIXELS 36


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t magenta = strip.Color(255,0,255);
uint32_t hotpink = strip.Color( 255,55,51 );
uint32_t yellow = strip.Color(255,215,0);
uint32_t cyan = strip.Color(0,255,255);
uint32_t orange = strip.Color(255,102,0);

uint32_t noColor =  strip.Color(0,0,0);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(255);
  setAllPixels( red, true );
}

void loop() {
  setAllPixels( yellow, true );
  delay(1000);
  clearAllPixels( true );
  delay(1000);
}

void setAllPixels( uint32_t color, boolean withShow ) {
  for ( int i=0; i < NUM_PIXELS; i++ ) {
    strip.setPixelColor(i, color );
  }
  if ( withShow ) {
    strip.show();
  }
}  

void clearAllPixels( boolean withShow ) {
    for ( int i=0; i < NUM_PIXELS; i++ ) {
      strip.setPixelColor(i, noColor );
    }
    if ( withShow ) {
      strip.show();
    }
}
