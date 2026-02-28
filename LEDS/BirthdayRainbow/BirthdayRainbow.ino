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


uint16_t currentPosition = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initStrip();
  //strip.setPixelColor(1, green);
  //strip.show();
  //gstrip.setPixelColor(0, strip.Color(255,0,0) );
}

void loop() {
  Serial.println("here");
  
  for ( int i = 0; i < pixelSize; i++ ) {
    Serial.println(i);
     if ( currentPosition == 16 ) {
       currentPosition = 0;
     }
      
     strip.setPixelColor( i, rainbow[currentPosition] );
     currentPosition++;
  }
  
  //strip.begin();
  //strip.setPixelColor(2, 255,0,0);
  //strip.setPixelColor(1, green);
  strip.show();  
  delay(100);
  
}

void initStrip() {
  strip.begin();
  strip.setBrightness(255);
  for ( int i = 0; i < pixelSize; i++ ) {
    strip.setPixelColor(i,0,0,0);
  }
  strip.show();
}



