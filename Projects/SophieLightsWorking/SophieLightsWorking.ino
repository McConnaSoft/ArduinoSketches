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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 6, NEO_GRB + NEO_KHZ800);

const unsigned long twoMinutes = 2 * 60 * 1000UL;

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t cyan = strip.Color(0,255,255);
uint32_t pink = strip.Color(255,0,127);
uint32_t purple = strip.Color(102,0,102);
uint32_t yellow = strip.Color(255,255,0);
uint32_t orange = strip.Color(204,102,0);

uint32_t noColor = strip.Color(0,0,0);

boolean alwaysOnSwitch = false;
boolean flashingSwitch = false;
boolean offSwitch = false;

boolean directionIsLeft = true;
boolean microphoneOnSwitch = false;
boolean rainbowColorOnSwitch = false;

uint32_t lastColor = noColor;
uint32_t currentColor = red;

long modeTime = 0;

int rainbowColorCount = 7;
uint32_t rainbowColors[7] = { purple, blue, green, yellow, orange, red, pink };
enum AnimationMode {
  ALWAYS_ON = 1, 
  FLASHING = 2, 
  LEFT_TO_RIGHT_BULLET = 3, 
  RIGHT_TO_LEFT_BULLET = 4, 
  BACK_AND_FORTH = 5,
  LEFT_TO_RIGHT_BULLETS = 6, 
  RIGHT_TO_LEFT_BULLETS = 7,  
  ALTERNATING = 8,
  MICROPHONE = 9,
  LIGHTS_OFF = 10 
};

enum SpecialMode {
  
  RAINBOWCOLOR_TOGGLE = 11
};

AnimationMode currentAnimationMode = 1;

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
  
  //attachInterrupt(0, change_handler, FALLING);

  for ( int i=0; i < bullets; i ++ ) {
    currentLTRPixelArray[i] = NUM_PIXELS - 1 - gap * i - bandLength * i;
    currentRTLPixelArray[i] = NUM_PIXELS - 1 - gap * i - bandLength * i;
  }
  
  alwaysOnSwitch = false;
  offSwitch = false;
  strip.begin();
  //pinMode(2, INPUT);
  strip.setBrightness(255);
  strip.setPixelColor(0, red);
  strip.show();
  
}

void change_handler() {
  Serial.println("Got somethin");
}

void loop() {
  
  //checkRadio();
  if ( millis() - modeTime > twoMinutes ) {
    Serial.println("Current mode expired");
     processValue( 1L );
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
      Serial.println("Unsupported animation mode");
      break;    
  }
  
}
  
void performAlwaysOn( ) {
  
  if ( !alwaysOnSwitch ) {
      alwaysOnSwitch = true;
      strip.setBrightness(255);
      if ( rainbowColorOnSwitch ) {
        Serial.println("Doing rainbow");
        fullRainbow( true );
      } else {
        setAllPixels( currentColor, true );
      }
  }
  
}

void performOff() {
  
  if ( !offSwitch ) {
    //if ( currentColor != lastColor ) {
    //  lastColor = currentColor;
    //}
    //currentColor = noColor;
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

long lastAnimationTime = 0;

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
     
  if ( millis() - lastAnimationTime > 80 ) {
     
    
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
     
  if ( millis() - lastAnimationTime > 80 ) {
     
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
     
  if ( millis() - lastAnimationTime > 40 ) {
     
    
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
  
  if ( millis() - lastAnimationTime > 80 ) {
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
  
  if ( millis() - lastAnimationTime > 40 ) {
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
  
  if ( millis() - lastAnimationTime > 500 ) {
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

long incomingTime = 0;

void handleIncoming() {

  if ( millis() - incomingTime < 500 ) {
    // Ignore this one
    return; 
  }

  incomingTime = millis();
  
  Serial.println("In with new method");
  int value = mySwitch.getReceivedValue();
  if (value <= 0 || value > 256256256 ) {
    Serial.print("Unknown encoding");
  } else {
    Serial.print("Received ");
    Serial.print(value);
    long value = mySwitch.getReceivedValue();
    Serial.print("Received L ");
    Serial.print(value);
    processValue( value );
  }    
  mySwitch.resetAvailable();
      
}

void processValue( long value ) {
  if ( value > 256 ) {
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
    Serial.println(value);
    if ( value <= 10 ) {
      modeTime = millis();
      alwaysOnSwitch = false;
      offSwitch = false;
      if ( value < 10 ) {
        colorReset();
      }
      if ( value != 9 ) {
        strip.setBrightness(255);
      }
      currentAnimationMode = value;
    } else if ( value == 11 ) {
      rainbowColorOnSwitch = !rainbowColorOnSwitch; 
      //if ( rainbowColorOnSwitch ) {
      //  Serial.println("Rainbow ON");
      //} else {
      //  Serial.println("Rainbow OFF" );
     // }
      alwaysOnSwitch = false;
      offSwitch = false;
    } 
  }  
}

