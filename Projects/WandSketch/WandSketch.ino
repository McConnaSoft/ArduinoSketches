#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define MAX_MODES 6
#define NUM_PIXELS 24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 1, NEO_GRB + NEO_KHZ800);



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
  pinMode(2, INPUT);
  
  strip.setBrightness(66);
  strip.show();

}


bool buttonWasDown = false;
long currentButtonDownTime = 0;
long quickClickCount = 0;
long firstClickTime = 0;

long currentMode = 0;

// the loop function runs over and over again forever
void loop() {

  /*
  digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);                       // wait for a second
  digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW
  delay(250);                       // wait for a second
  */
  bool buttonRead = digitalRead( 2 );
  long now = millis();
  long buttonPressDuration = now - currentButtonDownTime;
  if ( !buttonRead && !buttonWasDown ) {

    buttonWasDown = true; 
    currentButtonDownTime = millis();
  
  } else if ( !buttonRead && buttonWasDown ) {
   
    if ( buttonPressDuration >= 750 ) {
      // This is where we are going to do a spell
      performMode();   
    }
    
  } else if ( buttonRead ) {
      if ( buttonPressDuration < 650 ) {
        // This is were we will switch modes
        currentMode++;
        if ( currentMode >= MAX_MODES ) {
          currentMode = 0;
        }
        performModeIndicator();
      }
      buttonWasDown = false;
      currentButtonDownTime = 0;
  }
  

}

void performMode( ) {
  switch( currentMode ) {
    case 0:
      fastAlternate( red, pink );
      break;
    case 1:
      upAndDown( green, green, false );  
      break;
    case 2:
      ringSlide( pink );  
      break;
    case 3:
      upAndDown( yellow, orange, true );
      break; 
    case 4:
      fastAlternate( blue, cyan );
      break;     
    case 5:
      rainbowRingSlide();  
    default:
      break;   
  }
}

void performModeIndicator( ) {
  switch( currentMode ) {
    case 0:
      lightRing( 0, 500, red, true );     
      break;
    case 1:  
      lightRing( 1, 500, green, true );
      break;
    case 2:
      lightRing( 2, 500, pink, true );
      break;
    case 3:
      lightRing( 3, 500, yellow, true);
      break;   
    case 4:
      lightRing( 4, 500, blue, true ); 
      break;
    case 5:
      lightRing( 5, 500, orange, true );    
    default:
      break;   
  }
}

void rainbowRingSlide( ) {
  for ( int i = 0; i < 10; i++ ) {
      lightRing( 0, 70, purple, true );
      lightRing( 1, 70, blue, true );
      lightRing( 2, 70, green, true );
      lightRing( 3, 70, yellow, true );
      lightRing( 4, 70, orange, true );
      lightRing( 5, 70, red, true );
  }
}

void upAndDown( uint32_t color1, uint32_t color2, boolean leaveTrail ) {
  for ( int i = 0; i < NUM_PIXELS; i++ ) {
     if ( i % 2 == 0 ) {
        strip.setPixelColor(i, color1 );
     } else {
        strip.setPixelColor(i, color2 );
     }
     strip.show();
     delay(50);
     if ( !leaveTrail ) {
        strip.setPixelColor(i, noColor );
        strip.show();
     }
  }
  allOff();
}

void lightRing( int ringIndex, long duration, uint32_t color, bool turnOff) {
  strip.setPixelColor(ringIndex, color );
  strip.setPixelColor(NUM_PIXELS / 2 - ringIndex - 1, color );
  strip.setPixelColor(ringIndex + NUM_PIXELS / 2, color );
  strip.setPixelColor(NUM_PIXELS - ringIndex - 1, color ); 
  
  strip.show();
  delay( duration );
  if ( turnOff ) {
    allOff();
  }
}

void fastAlternate( uint32_t color1, uint32_t color2 ) {
  for ( int i = 0; i < 10; i++ ) {
    for ( int j = 0; j < 6; j++ ) {
      if ( i % 2 == 0 ) {
        if ( j % 2 == 0 ) {
          lightRing( j, 0, color1, false );
        } else {
          lightRing( j, 0, color2, false );  
        }
      } else {
        if ( j % 2 == 0 ) {
          lightRing( j, 0, color2, false );
        } else {
          lightRing( j, 0, color1, false );  
        }
      }
      delay(50);
    }
  }
  allOff();
}

void allMode( uint32_t color) {
  for ( int i = 0; i < 24; i++ ) {
    strip.setPixelColor(i, color );
  }
  strip.show();
  delay(2000);
  allOff();
}

void ringSlide( uint32_t color ) {
  for ( int i = 0; i < 5; i++ ) { 
    for( int j = 0; j < 6; j++ ) {
      lightRing( j, 50, color, true );
    }
  }    
}

void allOff() {
  for ( int i = 0; i < 24; i++ ) {
    strip.setPixelColor(i, noColor );
  }
  strip.show();
}


