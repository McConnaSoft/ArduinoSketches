#include <RCSwitch.h>
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define MAX_MODES 6
#define NUM_PIXELS 150

volatile Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 6, NEO_GRB + NEO_KHZ800);

void(* resetFunc) (void) = 0;

const unsigned long threeMinutes = 3 * 60 * 1000UL;

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t cyan = strip.Color(0,255,255);
uint32_t pink = strip.Color(255,0,127);
uint32_t purple = strip.Color(102,0,102);
uint32_t yellow = strip.Color(255,255,0);
uint32_t orange = strip.Color(204,102,0);

//long MAX_BRIGHTNESS = 255;

uint32_t noColor = strip.Color(0,0,0);

volatile boolean alwaysOnSwitch = false;
volatile boolean flashingSwitch = false;
volatile boolean offSwitch = false;

boolean directionIsLeft = true;
boolean microphoneOnSwitch = false;
boolean rainbowColorOnSwitch = false;

volatile uint32_t lastColor = noColor;
volatile uint32_t currentColor = red;


volatile long lastAnimationTime = 0;
volatile long modeTime = 0;
long BULLET_WAIT = 40;
long BULLET_WAIT_SLOW = 80;

int rainbowColorCount = 7;
uint32_t rainbowColors[7] = { purple, blue, green, yellow, orange, red, pink };

uint8_t brightnessLevels[5] = { 255, 122, 80, 60, 40 }; 
int currentBrightnessLevel = 0;

enum AnimationMode {
  ALWAYS_ON = 10001, 
  FLASHING = 10002, 
  LEFT_TO_RIGHT_BULLET = 10003, 
  RIGHT_TO_LEFT_BULLET = 10004, 
  BACK_AND_FORTH = 10005,
  LEFT_TO_RIGHT_BULLETS = 10006, 
  RIGHT_TO_LEFT_BULLETS = 10007,  
  ALTERNATING = 10008,
  MICROPHONE = 10009,
  LIGHTS_OFF = 10010 
};

enum SpecialMode {
  
  RAINBOWCOLOR_TOGGLE = 10011,
  BRIGHTNESS_TOGGLE = 10012,
  ARDUINO_RESET = 10013
};

volatile AnimationMode currentAnimationMode = 10001;

int bandLength=14;
int bullets = 5;
int gap = 16;
int currentLTRPixelArray[5];
int currentRTLPixelArray[5];

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.setPulseLength(350);
  mySwitch.setReceiveTolerance(100);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  mySwitch.setCustomHandlerFunction( handleIncoming );

  for ( int i=0; i < bullets; i ++ ) {
    currentLTRPixelArray[i] = NUM_PIXELS - 1 - gap * i - bandLength * i;
    currentRTLPixelArray[i] = NUM_PIXELS - 1 - gap * i - bandLength * i;
  }
  
  alwaysOnSwitch = false;
  offSwitch = false;
  strip.begin();
  strip.setBrightness(brightnessLevels[currentBrightnessLevel]);
  strip.setPixelColor(0, red);
  strip.show();

  Serial.print("Ready...");

}

void loop() {
  
  if ( millis() - modeTime > threeMinutes && currentAnimationMode != 10010 ) {
     //Serial.println("Current mode expired");
     processValue( 10001L );
  }
  
  switch ( currentAnimationMode ) {
    case ALWAYS_ON:
      performAlwaysOn();
      break;
    case FLASHING:
      performFlashing();
      break;
    case LEFT_TO_RIGHT_BULLET:
      performLeftToRightBullet();
      break;
    case RIGHT_TO_LEFT_BULLET:
      performRightToLeftBullet();
      break;  
    case BACK_AND_FORTH:
      performBounceBullet();
      break;  
    case LEFT_TO_RIGHT_BULLETS:
      performLeftToRightBullets();
      break;
    case RIGHT_TO_LEFT_BULLETS:
      performRightToLeftBullets();
      break;    
    case ALTERNATING:
      performAlternating();  
      break;
    case MICROPHONE:
      performBrightnessBySound(); 
      break; 
    case LIGHTS_OFF:
      performOff();
      break;  
    default:
      //Serial.println("Unsupported animation mode");
      break;    
  }
  
}
  
void performAlwaysOn( ) {
  
  if ( !alwaysOnSwitch ) {
      alwaysOnSwitch = true;
      strip.setBrightness(brightnessLevels[currentBrightnessLevel]);
      if ( rainbowColorOnSwitch ) {
        //Serial.println("Doing rainbow");
        fullRainbow( true );
      } else {
        setAllPixels( currentColor, true );
      }
  }
  
}

void performOff() {
  
  if ( !offSwitch ) {
    setAllPixels( noColor, true );
    offSwitch = true;
    alwaysOnSwitch = false;
  }
  
}

void fullRainbow( boolean withShow ) {
  int sectionSize = NUM_PIXELS / rainbowColorCount;
  for ( int i=0; i < NUM_PIXELS; i++ ) {
    strip.setPixelColor( i, rainbowColors[i / sectionSize] );
  }
  if ( withShow) {
    strip.show();
  }
}


void performFlashing() {
  
  if ( millis() - lastAnimationTime > 500 ) {
    
    if ( flashingSwitch ) {
      if ( rainbowColorOnSwitch ) {
        fullRainbow( true );
      } else {
        setAllPixels( currentColor, true );
      }
    } else {
      clearAllPixels( true );  
    }
  
    flashingSwitch = !flashingSwitch;
    lastAnimationTime = millis();
    
  }
  
}

void colorReset() {
    if ( currentColor == noColor ) {
      currentColor = lastColor;
    }
}

int currentLTRPixel = NUM_PIXELS - 1;
void performLeftToRightBullet() {
     
  if ( millis() - lastAnimationTime > BULLET_WAIT_SLOW ) {
     
    
     clearAllPixels( false );
     for ( int i=0; i< bandLength; i++ ) {
        if ( rainbowColorOnSwitch ) {
          strip.setPixelColor( (currentLTRPixel + i > NUM_PIXELS -1 ) ? (currentLTRPixel + i) - NUM_PIXELS: currentLTRPixel + i, rainbowColors[ i / 2] );
        } else {
          strip.setPixelColor( (currentLTRPixel + i > NUM_PIXELS -1 ) ? (currentLTRPixel + i) - NUM_PIXELS: currentLTRPixel + i, currentColor ); 
        }
     }
     strip.show();
     if ( currentLTRPixel == 0 ) {
        currentLTRPixel = NUM_PIXELS - 1;
     } else {
        currentLTRPixel--;
     }
     
     lastAnimationTime = millis();
  }
  
}

int currentBouncePixel = NUM_PIXELS - 1 - bandLength;
void performBounceBullet() {
     
  if ( millis() - lastAnimationTime > BULLET_WAIT_SLOW ) {
     
     clearAllPixels( false );
     for ( int i=0; i< bandLength; i++ ) {
        if ( rainbowColorOnSwitch ) {
          if ( directionIsLeft ) {
            strip.setPixelColor( (currentBouncePixel + i > NUM_PIXELS -1 ) ? (currentBouncePixel + i) - NUM_PIXELS: currentBouncePixel + i, rainbowColors[ i / 2] );
          } else {
            strip.setPixelColor( (currentBouncePixel - i < 0 ) ? (currentBouncePixel + i) + NUM_PIXELS: currentBouncePixel - i, rainbowColors[ i / 2] );
          }
        } else {
          if ( directionIsLeft ) {
            strip.setPixelColor( (currentBouncePixel + i > NUM_PIXELS -1 ) ? (currentBouncePixel + i) - NUM_PIXELS: currentBouncePixel + i, currentColor ); 
          } else {
            strip.setPixelColor( (currentBouncePixel - i < 0 ) ? (currentBouncePixel + i) + NUM_PIXELS: currentBouncePixel - i, currentColor );
          }
        }
     }
     strip.show();
     if ( directionIsLeft ) {
       if ( currentBouncePixel-bandLength + 1 == 0 ) {
          directionIsLeft = false;
       } else {
          currentBouncePixel--;
       }
     } else {
       if ( currentBouncePixel == NUM_PIXELS - 1 - bandLength) {
          directionIsLeft = true;
       } else {
          currentBouncePixel++;
       }
     }
     
     lastAnimationTime = millis();
  }
  
}

// 5 bullets of 14 spaced by 16 pixels

void performLeftToRightBullets() { 
     
  if ( millis() - lastAnimationTime > BULLET_WAIT ) {
     
    
     clearAllPixels( false );
     for ( int i=0; i< bandLength; i++ ) {
        for ( int j=0; j < bullets; j++ ) {
          if ( rainbowColorOnSwitch ) {
            strip.setPixelColor( (currentLTRPixelArray[j] + i > NUM_PIXELS -1 ) ? (currentLTRPixelArray[j] + i) - NUM_PIXELS: currentLTRPixelArray[j] + i, rainbowColors[ i / 2] );
          } else {
            strip.setPixelColor( (currentLTRPixelArray[j] + i > NUM_PIXELS -1 ) ? (currentLTRPixelArray[j] + i) - NUM_PIXELS: currentLTRPixelArray[j] + i, currentColor ); 
          }
        }
     }
     strip.show();
     for ( int j=0; j < bullets; j++ ) {
       if ( currentLTRPixelArray[j] == 0 ) {
          currentLTRPixelArray[j] = NUM_PIXELS - 1;
       } else {
          currentLTRPixelArray[j]--;
       }
     }
     
     lastAnimationTime = millis();
  }
  
}


int currentRTLPixel = NUM_PIXELS - 1;
void performRightToLeftBullet() {
  
  if ( millis() - lastAnimationTime > BULLET_WAIT_SLOW ) {
    clearAllPixels( false );
    for ( int i=0; i< bandLength; i++ ) {
      if ( rainbowColorOnSwitch ) {
         strip.setPixelColor( (currentRTLPixel - i < 0 ) ? (currentRTLPixel + i) + NUM_PIXELS: currentRTLPixel - i, rainbowColors[ i / 2] );
      } else {
        strip.setPixelColor( (currentRTLPixel - i < 0 ) ? (currentRTLPixel + i) + NUM_PIXELS: currentRTLPixel - i, currentColor ); 
      }
    }
    strip.show();
    if ( currentRTLPixel == NUM_PIXELS ) {
      currentRTLPixel = 0;
    } else {
      currentRTLPixel++;
    }
  
    lastAnimationTime = millis();
  }
  
}

void performRightToLeftBullets() {
  
  if ( millis() - lastAnimationTime > BULLET_WAIT ) {
    clearAllPixels( false );
    for ( int i=0; i< bandLength; i++ ) {
      for ( int j=0; j < bullets; j++ ) {
        if ( rainbowColorOnSwitch ) {
         strip.setPixelColor( (currentRTLPixelArray[j] - i < 0  ) ? (currentRTLPixelArray[j] + i) + NUM_PIXELS: currentRTLPixelArray[j] - i, rainbowColors[ i / 2] );
        } else {
          strip.setPixelColor( (currentRTLPixelArray[j] - i < 0  ) ? (currentRTLPixelArray[j] + i) + NUM_PIXELS: currentRTLPixelArray[j] - i, currentColor ); 
        }
      }
    }
    strip.show();
    for ( int j=0; j < bullets; j++ ) {
      if ( currentRTLPixelArray[j] == NUM_PIXELS ) {
        currentRTLPixelArray[j] = 0;
      } else {
        currentRTLPixelArray[j]++;
      }
    }
  
    lastAnimationTime = millis();
  }
  
}




int altFlip = 0;
void performAlternating() {
  
  if ( millis() - lastAnimationTime > 150 ) {
    if ( altFlip == 0 ) {
      altFlip = 1;
      for ( int i=0; i < NUM_PIXELS; i ++ ) {
        strip.setPixelColor( i, i%2==0 ? currentColor : lastColor);
      }
    } else {
      altFlip = 0;
      for ( int i=0; i < NUM_PIXELS; i ++ ) {
        strip.setPixelColor( i, i%2==0 ? lastColor : currentColor);
      }
    }
    strip.show();
    lastAnimationTime = millis();
  }
  
}

void performBrightnessBySound() {

  if ( millis() - lastAnimationTime > 50 ) {
    
  
    unsigned long startMillis= millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level
  
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;
    unsigned int sample;
     // collect data for 50 mS
     while (millis() - startMillis < 50)
     {
        
        sample = analogRead(A0);
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
    //Serial.println(peakToPeak);
    if ( rainbowColorOnSwitch ) {
      fullRainbow( false );
    } else {
      setAllPixels( currentColor, false );
    }
    strip.setBrightness( map(peakToPeak,30,1024,0,255) );
    strip.show();

    lastAnimationTime = millis();
  }
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

volatile long incomingTime = 0;

void handleIncoming() {

  if ( millis() - incomingTime < 500 ) {
    // Ignore this one
    return; 
  }

  incomingTime = millis();

  //Serial.println("In with new method");
  long value = mySwitch.getReceivedValue();
  //Serial.println( value );
  if (value < 10000 || value > 256256256 ) {
    Serial.print("Unknown encoding");
    mySwitch.resetAvailable();
    // Assume Always On
    // processValue( 1L );
  } else {
    //Serial.print("Received: ");
    //long value = mySwitch.getReceivedValue();
    //Serial.println( value );
    mySwitch.resetAvailable();
    processValue( value );
  }    
  
      
}

void processValue( long value ) {
  if ( value > 10020 ) {
    //Serial.println(value);
    //256-256-256
    //Serial.println("It is a color");
    //Serial.print("Red: ");
    //Serial.println( value / 1000000 );
    //Serial.print("Green: ");
    //Serial.println( (value / 1000) % 1000  );
    //Serial.print("Blue: ");
    //Serial.println( value % 1000 );
    lastColor = currentColor;
    alwaysOnSwitch = false;
    currentColor = strip.Color( value / 100000, (value /1000) % 1000, value % 1000);

  } else {
        
    //Serial.println("It is a mode");
    //Serial.print("Mode: ");
    //Serial.println(value);
    if ( value <= 10010 ) {
      modeTime = millis();
      alwaysOnSwitch = false;
      offSwitch = false;
      if ( value < 10010 ) {
        colorReset();
      }
      if ( value != 10009 ) {
        strip.setBrightness(brightnessLevels[currentBrightnessLevel]);
      }
      currentAnimationMode = value;
    } else if ( value == 10011 ) {
      rainbowColorOnSwitch = !rainbowColorOnSwitch; 
      alwaysOnSwitch = false;
      offSwitch = false;
    } else if ( value == 10012 ) {
      currentBrightnessLevel++;
      if ( currentBrightnessLevel >= 5 ) {
        currentBrightnessLevel = 0;
      }
      strip.setBrightness( brightnessLevels[currentBrightnessLevel] );
      strip.show();
    } else if ( value == 10013 ) {
      resetFunc();
    }
  }  
}




