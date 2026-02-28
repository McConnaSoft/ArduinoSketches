#include <Adafruit_NeoPixel.h>

#define LED_DATA_PIN 5
#define PINCOUNT 74 //16

#define BUTTON_1 8
#define BUTTON_2 9
#define BUTTON_3 10
#define BUTTON_4 11


#define PIR_PIN     6


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PINCOUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

int currentButton = BUTTON_1;

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t magenta = strip.Color(255,0,255);
//uint32_t hotpink = strip.Color(255,147,105);
uint32_t hotpink = strip.Color( 255,55,51 );
uint32_t yellow = strip.Color(255,215,0);
uint32_t cyan = strip.Color(0,255,255);
uint32_t orange = strip.Color(255,102,0);

uint32_t flip = 0;

void setup() {
  Serial.begin(9600);
  
  strip.begin();
  
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  
  pinMode(PIR_PIN, INPUT);

}

void loop() {
  
  checkForButtons(); 
  switch( currentButton ) {
    case BUTTON_1:
      performButton1();
      break;
    case BUTTON_2:
      performButton2();
      break;
    case BUTTON_3:
      performButton3();
      break;
    case BUTTON_4:
      performButton4();
      break;    
  }
  
  performDelay();

}

void checkForButtons() {
    
    for ( int i=BUTTON_1; i <= BUTTON_4; i++) { 
      if ( !digitalRead( i ) ) {
        Serial.print( "Button " ); 
        Serial.print( i );
        Serial.println( " - pressed" );
        currentButton = i;
        break;
      }
    } 
}

void performDelay() {
  switch( currentButton ) {
    case BUTTON_1:
      delay(40); // Was 30
      break;
    case BUTTON_2:
      delay(300);
      break;
    case BUTTON_3:
      delay(0);
      break;
    case BUTTON_4:
      delay(0);
      break;    
  }
}

int currentPixel = PINCOUNT -1;
int bandLength = 7;

void performButton1() {
  resetLEDS();
  strip.setBrightness(65);
  
  strip.setPixelColor( currentPixel, red );
  strip.setPixelColor( (currentPixel + 1 > PINCOUNT - 1 ) ? (currentPixel + 1) - PINCOUNT: currentPixel + 1, red );
  strip.setPixelColor( (currentPixel + 2 > PINCOUNT - 1 ) ? (currentPixel + 2) - PINCOUNT: currentPixel + 2, blue );
  strip.setPixelColor( (currentPixel + 3 > PINCOUNT - 1 ) ? (currentPixel + 3) - PINCOUNT: currentPixel + 3, blue );
  strip.setPixelColor( (currentPixel + 4 > PINCOUNT - 1 ) ? (currentPixel + 4) - PINCOUNT: currentPixel + 4, cyan );
  strip.setPixelColor( (currentPixel + 5 > PINCOUNT - 1 ) ? (currentPixel + 5) - PINCOUNT: currentPixel + 5, cyan );
  strip.setPixelColor( (currentPixel + 6 > PINCOUNT - 1 ) ? (currentPixel + 6) - PINCOUNT: currentPixel + 6, green );
  strip.setPixelColor( (currentPixel + 7 > PINCOUNT - 1 ) ? (currentPixel + 7) - PINCOUNT: currentPixel + 7, green );
  strip.setPixelColor( (currentPixel + 8 > PINCOUNT - 1 ) ? (currentPixel + 8) - PINCOUNT: currentPixel + 8, hotpink );
  strip.setPixelColor( (currentPixel + 9 > PINCOUNT - 1 ) ? (currentPixel + 9) - PINCOUNT: currentPixel + 9, hotpink );
  strip.setPixelColor( (currentPixel + 10 > PINCOUNT - 1 ) ? (currentPixel + 10) - PINCOUNT: currentPixel + 10, yellow );
  strip.setPixelColor( (currentPixel + 11 > PINCOUNT - 1 ) ? (currentPixel + 11) - PINCOUNT: currentPixel + 11, yellow );
  strip.setPixelColor( (currentPixel + 12 > PINCOUNT - 1 ) ? (currentPixel + 12) - PINCOUNT: currentPixel + 12, orange );
  strip.setPixelColor( (currentPixel + 13 > PINCOUNT - 1 ) ? (currentPixel + 13) - PINCOUNT: currentPixel + 13, orange );
  strip.show();
  
  if ( currentPixel == 0 ) {
    currentPixel = PINCOUNT - 1;
  } else {
    currentPixel--;
  }

}

void performButton2() {
  resetLEDS();
  strip.setBrightness(65);
  if ( flip == 0 ) {
  
    for ( int i=0; i<150; i=i+2 ) {
      strip.setPixelColor( i, red );
      strip.setPixelColor( i + 1, blue );
    }
    flip = 1;
  } else {
    for ( int i=0; i<150; i=i+2 ) {
      strip.setPixelColor( i, blue );
      strip.setPixelColor( i + 1, red );
    }
    flip = 0;
  }
  
  strip.show();
 
}

void performButton3()
{
  //resetLEDS();
  for (int i=0; i < PINCOUNT; i++ )
  {
     strip.setPixelColor(i,blue);
  }
  
  
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int sample;
   // collect data for 50 mS
   while (millis() - startMillis < 50)
   {
      sample = analogRead(A1);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  if ( peakToPeak  < 30 ) {
    peakToPeak = 30;
  } 
  Serial.println(peakToPeak);
  strip.setBrightness( map(peakToPeak,30,500,0,255) );
  strip.show();
}

void performButton4()
{
  strip.setBrightness(255);
  if ( digitalRead( PIR_PIN) == LOW ) {
    resetLEDS();
  } else {
    for (int i=0; i < PINCOUNT; i++ )
    {
       strip.setPixelColor(i,red);
    }
  }
  
  strip.show();

}

void resetLEDS() {
  for ( int i=0; i < PINCOUNT; i++ ) 
  {
      strip.setPixelColor(i,0,0,0);
  } 
  //strip.show();
}
