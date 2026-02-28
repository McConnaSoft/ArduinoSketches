
#include <Adafruit_NeoPixel.h>

#define DEBUG false
#define Serial if(DEBUG)Serial

#define MAX_MODES 7
#define MAX_MODE_COLORS 9

#define PIXEL_PIN 7
#define MODE_PIN 5
#define COLOR_PIN 6

#define NUM_LEDS 10

enum BulletMode {
  WRAP_DOWN = 10001, 
  WRAP_UP = 10002, 
  BOUNCE = 10003
};



// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t cyan = strip.Color(0,255,255);
uint32_t pink = strip.Color(255,0,127);
uint32_t purple = strip.Color(102,0,102);
uint32_t yellow = strip.Color(255,255,0);
uint32_t orange = strip.Color(204,102,0);
uint32_t white = strip.Color(255,255,255);

uint32_t noColor = strip.Color(0,0,0);

uint32_t modeIndicatorColors[MAX_MODES] = {red, green, blue, pink, purple, yellow};
uint32_t modeColors[MAX_MODE_COLORS] = {red, green, blue, cyan, pink, purple, yellow, orange, white};
uint32_t modeAltColors[MAX_MODE_COLORS] = {blue, yellow, orange, red, white, green, cyan, pink, noColor};


const int MONITORED_PINS = 2;

int bouncedPins[MONITORED_PINS] = { MODE_PIN, COLOR_PIN };
boolean buttonStates[MONITORED_PINS] = {HIGH, HIGH};

int brightness = 255;
volatile uint32_t lastColor = noColor;

volatile long lastAnimationTime = 0;

void (*buttonPressedHandlers[MONITORED_PINS])();

long currentMode = 0;
long currentColor = 0;

void setup() {
  Serial.begin(9600);

  // PIN SETUP
  pinMode( MODE_PIN, INPUT_PULLUP);
  pinMode( COLOR_PIN, INPUT_PULLUP);
  buttonPressedHandlers[0] = modeButtonPressed;
  buttonPressedHandlers[1] = colorButtonPressed;

  strip.begin();  

  performModeIndicator();

}

void loop() {

  debounceAndHandleButtons();

  performMode();


}

void performMode( ) {
  switch( currentMode ) {
    case 0:
      performBrightnessBySound();
      break;
    case 1:
      performAlwaysOn(); 
      delay(100);
      break;
    case 2:
      performFade();
      break; 
    case 3:
      performAlternating();
      break;
    case 4:
      performBullet( WRAP_UP );
      break;
    case 5:
      performBullet( WRAP_DOWN );
      break;
    case 6:
      performBullet( BOUNCE );    
    default:
      break;   
  }
}

void modeButtonPressed() {
  Serial.println("Mode button was pressed");
  currentMode++;
  if ( currentMode >= MAX_MODES ) {
    currentMode = 0;
  }
  switch( currentMode ) {
    case 0:
      Serial.println("SOUND MODE");
      break;
    case 1:
      Serial.println("ALWAYS ON MODE");
      break;
    case 2:
      Serial.println("FADE MODE");
      break;
    case 3:
      Serial.println("ALT COLOR MODE");
      break;  
    case 4:
      Serial.println("BULLET MODE - UP");  
      break;
    case 5:   
      Serial.println("BULLET MODE - DOWN");  
      break;
    case 6: 
      Serial.println("BULLET MODE - BOUNCE");  
      break;  
    default:
      break;   
  }
  performModeIndicator();
}

void colorButtonPressed() {
  Serial.println("Color button was pressed");
  currentColor++;
  if ( currentColor >= MAX_MODE_COLORS ) {
    currentColor = 0;
  }

}

void performModeIndicator( ) {
   strip.setBrightness(255);
   clearAllPixels( false );
   strip.setPixelColor(currentMode, white);
   strip.show();
   delay(250);
   clearAllPixels( true );
}

void changeBrightness() {
   int potValue = analogRead(A2);
   strip.setBrightness( map(potValue,0,1024,0,255) );
}




void debounceAndHandleButtons()
{

  boolean currentReads[MONITORED_PINS];
  boolean anyButtonStateChanged = false; 

  for( int i = 0; i < MONITORED_PINS; i++ ) {
    currentReads[i] = digitalRead( bouncedPins[i] );
    if ( currentReads[i] != buttonStates[i] ) {
      anyButtonStateChanged = true;
    }
  }

  if ( anyButtonStateChanged ) {
    delay( 50 );  // debounce delay

    for( int i = 0; i < MONITORED_PINS; i++ ) {
      currentReads[i] = digitalRead( bouncedPins[i] );
    }
    for( int i = 0; i < MONITORED_PINS; i++ ) {
      if ( currentReads[i] == LOW ) {
        buttonPressedHandlers[i]();
      }
    }
  }
     
  for( int i = 0; i < MONITORED_PINS; i++ ) {
    buttonStates[i] = currentReads[i];
  }
  
}

int peakToPeak = 0;
int static_variable = 500;

// Sensitivity goes from most to least
int sensitivityCeiling[] = {300, 350, 400, 500, 600};
int sensitivityFloor[] = {20, 30, 40, 50, 50};
int biggest = 0;
int biggestPeak = 0;

void performBrightnessBySound() {
  int sensitivityLevel = map(analogRead(A2), 0, 1024, 4, 0);

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int sample;
  // collect data for 50 mS
  unsigned long startMillis = millis();
  while (millis() - startMillis < 50) // 50ms is the sample rate
  {
    sample = analogRead(A0);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
        if (signalMax > biggest) {
          biggest = signalMax;
        }
      }

      if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  int peakFloor = sensitivityFloor[sensitivityLevel];
  if (peakToPeak < peakFloor) {
    peakToPeak = peakFloor;
  }
  if (peakToPeak > biggestPeak) {
    biggestPeak = peakToPeak;
  }

  Serial.print("Max:");
  Serial.print(signalMax);
  Serial.print(",");
  Serial.print("Min:");
  Serial.print(signalMin);
  Serial.print(",");
  Serial.print("Peak:");
  Serial.print(peakToPeak);
  Serial.print(",");
  Serial.print("BiggestPeak:");
  Serial.println(biggestPeak);

  setAllPixels(modeColors[currentColor], false);
  strip.setBrightness(map(peakToPeak, peakFloor, sensitivityCeiling[sensitivityLevel], 0, 255));  //(200 was 1024)
  strip.show();
}

void performAlwaysOn() {
  changeBrightness();
  if (millis() - lastAnimationTime > 50) {
    setAllPixels(modeColors[currentColor], true);
    lastAnimationTime = millis();
  }
}

void performPotMonitor() {
  int reading = analogRead(A2);
  Serial.print("Reading:");
  Serial.println(reading);
}

int currentBulletPos = NUM_LEDS - 1;
boolean bulletDown = true;

void performBullet( BulletMode bulletMode ) {

  int potValue = analogRead(A2);
  int animationMillis = map(potValue,0,1024,120,20);


  if ( millis() - lastAnimationTime > animationMillis ) {
    strip.setBrightness(255);
    clearAllPixels( false );
    
    switch( bulletMode ) {
      case WRAP_DOWN:
        sendBulletDown(); 
        currentBulletPos = ( currentBulletPos == 0 )?(NUM_LEDS - 1):(currentBulletPos -1);
        break;
      case WRAP_UP:
        sendBulletUp();
        currentBulletPos = ( currentBulletPos == NUM_LEDS )?0:(currentBulletPos+1);
        break;
      case BOUNCE:
        if (bulletDown) sendBulletDown(); else sendBulletUp();
        if (bulletDown) {
           if ( currentBulletPos == 0 ) {
             bulletDown = false;
             currentBulletPos++;
           } else {
             currentBulletPos--;
           }
        } else {
           if ( currentBulletPos == NUM_LEDS - 1 ) {
             bulletDown = true;
             currentBulletPos--;
           } else {
             currentBulletPos++;
           }
        } 
        break;
      default:
        break;
        
    }
    
    
    strip.show();
    lastAnimationTime = millis();
  }

}

void sendBulletDown() {
  strip.setPixelColor( (currentBulletPos > NUM_LEDS -1 ) ? currentBulletPos - NUM_LEDS: currentBulletPos, modeColors[currentColor] );
}

void sendBulletUp() {
  strip.setPixelColor( (currentBulletPos < 0 ) ? (currentBulletPos) + NUM_LEDS: currentBulletPos, modeColors[currentColor] );
}

boolean flip = false;

void performAlternating() {

  strip.setBrightness(255);
  int potValue = analogRead(A2);
  int animationMillis = map(potValue,0,1024,500,80);

  if ( millis() - lastAnimationTime > animationMillis ) {
    clearAllPixels(false);
    for ( int i=0; i < NUM_LEDS ; i=i+2 ) {
      strip.setPixelColor( i, (flip == 1)?modeColors[currentColor]:modeAltColors[currentColor] );
      strip.setPixelColor( i + 1, (flip ==1)?modeAltColors[currentColor]:modeColors[currentColor] );
    }
  
    strip.show();
    lastAnimationTime = millis();
    flip = !flip;
  }

}

boolean brightnessUp = true;
int speedCheck = 0;
void performFade() {
  
  int potValue = analogRead(A2);
  int animationMillis = map(potValue,0,1024,30,0);

  if ( millis() - lastAnimationTime > animationMillis ) {
    strip.setBrightness(brightness);
    setAllPixels(modeColors[currentColor], false);
    strip.show();
    if ( brightnessUp ) {
      brightness = brightness + map(potValue,0,1024,1,4);
      if ( brightness > 255 ) {
        brightness = 254;
        brightnessUp = false;
      }
    } else {
      brightness = brightness - map(potValue,0,1024,1,4);
      if ( brightness < 0 ) {
        brightness = 1;
        brightnessUp = true;
      }
    } 


    lastAnimationTime = millis();
  }

}

void setAllPixels( uint32_t color, boolean withShow ) {
  for ( int i=0; i < NUM_LEDS; i++ ) {
    strip.setPixelColor(i, color );
  }
  if ( withShow ) {
    strip.show();
  }
}

void clearAllPixels( boolean withShow ) {
    for ( int i=0; i < NUM_LEDS; i++ ) {
      strip.setPixelColor(i, noColor );
    }
    if ( withShow ) {
      strip.show();
    }
}



