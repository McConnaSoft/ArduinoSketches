#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define NUM_PIXELS 12
#define LED_PIN 11
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);



uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t cyan = strip.Color(0,255,255);
uint32_t pink = strip.Color(255,0,127);
uint32_t purple = strip.Color(102,0,102);
uint32_t yellow = strip.Color(255,255,0);
uint32_t orange = strip.Color(204,102,0);

uint32_t noColor = strip.Color(0,0,0);

// the setup function runs once when you press reset or power the board
void setup() {
  
  strip.begin();
  strip.setBrightness(66);
  strip.show();
  
}

void loop() {
  strip.setPixelColor(0,pink);
  strip.show();  
}
