
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

const int zAxisPin = A0;
const int yAxisPin = A1;
const int xAxisPin = A2;

const int coasterLEDPin = 11;
const int NUM_COASTER_PIXELS = 13;
const int scale = 3;



// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_COASTER_PIXELS, coasterLEDPin, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t black = strip.Color(0, 0, 0);
uint32_t white = strip.Color(255, 255, 255);
uint32_t light_blue = strip.Color(176, 224, 230);
uint32_t middle_blue = strip.Color(0, 191, 255);

uint32_t cyan = strip.Color(0,255,255);
uint32_t pink = strip.Color(255,0,127);
uint32_t purple = strip.Color(102,0,102);
uint32_t yellow = strip.Color(255,255,0);
uint32_t orange = strip.Color(204,102,0);

uint32_t rb1 = purple;
uint32_t rb2 = blue;
uint32_t rb3 = green;
uint32_t rb4 = yellow;
uint32_t rb5 = orange;
uint32_t rb6 = red;

uint32_t rb[6] = {rb1, rb2, rb3, rb4, rb5, rb6};


enum CoasterMode {
  HOT_PINK, // 0
  SIDE_SIDE, // 1
  CHRISTMAS, // 2
  IN_OUT, //3
  SHIMMER, //4
  RAINBOW //5
};

const int NUM_MODES = 6;

CoasterMode currentMode;

int currentPosition = 0;
int currentDirection = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.println("Starting...");
  unsigned int lastModeUsed = EEPROMReadInt(0);
  Serial.println("The last mode used was: ");
  Serial.println(lastModeUsed);

  currentMode = lastModeUsed + 1;
  if ( currentMode >= NUM_MODES ) {
    currentMode = 0;
  }
  EEPROMWriteInt(0,currentMode);
  
  strip.begin();
  uint16_t n = strip.numPixels();
  strip.setBrightness(64);

  Serial.print("The number of pixels are: " ); Serial.println( n );
  Serial.println("Current Mode: ");
  Serial.println(currentMode);
  /*
  strip.setPixelColor(0, green);
  strip.setPixelColor(1, blue);
  strip.setPixelColor(2, light_blue);
  strip.setPixelColor(3, green);
  strip.setPixelColor(4, blue);
  strip.setPixelColor(5, red);
  strip.setPixelColor(6 , green);
  strip.setPixelColor(7 , blue);
  strip.setPixelColor(8, green);
  strip.setPixelColor(9, blue);
  strip.setPixelColor(10, light_blue);
  strip.setPixelColor(11, green);
  strip.setPixelColor(12, blue);
  */
}

void loop() {
  switch( currentMode ) {
    case 0:
      performHotPink();
      break;
    case 1:
      performSideSide();
      break;
    case 2:
      performChristmas();
      break;
    case 3:
      performInOut();
      break;
    case 4:
      performShimmer();
      break;
    case 5:
      performRainbow();
      break;
    default:
      //Serial.println("FATAL: Unsupported state!");
      break;    
  }

}

void performHotPink() {
  
  setAllPixels( pink, true );
  delay(3000);
  
}

void performSideSide() {
  
  clearAllPixels( true );
  strip.setPixelColor( currentPosition, blue );
  strip.show();
  currentPosition = currentPosition + 1 * currentDirection;
  if ( currentPosition < 0 || currentPosition >= NUM_COASTER_PIXELS )
  {
    currentDirection = currentDirection * -1;
    if ( currentPosition < 0 ) {
      currentPosition = 1;
    } else if ( currentPosition >= NUM_COASTER_PIXELS ) {
      currentPosition = NUM_COASTER_PIXELS - 2;
    }
  }
  
  delay(100);
  
}

void performInOut() {
  clearAllPixels( true );
  strip.setPixelColor( currentPosition, orange );
  strip.setPixelColor( (NUM_COASTER_PIXELS - 1) - currentPosition, orange );
  strip.show();

  currentPosition = currentPosition + 1 * currentDirection;
  if ( currentPosition < 0 || currentPosition == 7 ) {
     currentDirection = currentDirection * -1;
    if ( currentPosition < 0 ) {
      currentPosition = 1;
    } else if ( currentPosition == 7 ) {
      currentPosition = 5;
    }
  }
 
  delay(100);
}

void performChristmas() {
  
  for ( int i=0; i < NUM_COASTER_PIXELS; i++ ) {
    strip.setPixelColor(i ,i % 2 == 0 ? red : green );
  }
  strip.show();
  delay(700);
  
  for ( int i=0; i < NUM_COASTER_PIXELS; i++ ) {
    strip.setPixelColor(i ,i % 2 == 1 ? red : green );
  }
  strip.show();
  delay(700);
  
}

void performRainbow() {

  // Do first row 
  strip.setPixelColor(0, rb[currentPosition] );
  strip.setPixelColor(6, rb[currentPosition] );
  strip.setPixelColor(12,rb[currentPosition] );
  rainbowPositionIncrementor();
  strip.setPixelColor(1, rb[currentPosition] );
  strip.setPixelColor(5, rb[currentPosition] );
  strip.setPixelColor(7, rb[currentPosition] );
  strip.setPixelColor(11, rb[currentPosition] );
  rainbowPositionIncrementor();
  strip.setPixelColor(2, rb[currentPosition] );
  strip.setPixelColor(4, rb[currentPosition] );
  strip.setPixelColor(8, rb[currentPosition] );
  strip.setPixelColor(10, rb[currentPosition] );
  rainbowPositionIncrementor();
  strip.setPixelColor(3, rb[currentPosition] );
  strip.setPixelColor(9, rb[currentPosition] );
  rainbowPositionIncrementor();
  strip.show();
  delay(150);
}

int rainbowPositionIncrementor() {
  currentPosition++;
  if ( currentPosition == 5 ) {
    currentPosition = 0;
  }
}

void performShimmer() {
    for ( int i=0; i < NUM_COASTER_PIXELS; i++ ) {
    strip.setPixelColor(i ,i % 2 == 0 ? blue : light_blue );
  }
  strip.show();
  delay(80);
  
  for ( int i=0; i < NUM_COASTER_PIXELS; i++ ) {
    strip.setPixelColor(i ,i % 2 == 1 ? blue : light_blue );
  }
  strip.show();
  delay(80);
}


void setAllPixels( uint32_t color, boolean withShow ) {
  for ( int i=0; i < NUM_COASTER_PIXELS; i++ ) {
    strip.setPixelColor(i, color );
  }
  if ( withShow ) {
    strip.show();
  }
}  

void clearAllPixels( boolean withShow ) {
    for ( int i=0; i < NUM_COASTER_PIXELS; i++ ) {
      strip.setPixelColor(i, black );
    }
    if ( withShow ) {
      strip.show();
    }
}

/* EEPROM Functions
 */

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
     {
     byte lowByte = ((p_value >> 0) & 0xFF);
     byte highByte = ((p_value >> 8) & 0xFF);

     EEPROM.write(p_address, lowByte);
     EEPROM.write(p_address + 1, highByte);
     }

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
     {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
     }
 
