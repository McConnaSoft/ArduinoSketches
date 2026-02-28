#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

const int zAxisPin = A0;
const int yAxisPin = A1;
const int xAxisPin = A2;

const int leftArmLEDPin = 11;
const int NUM_ARM_PIXELS = 9;
const int scale = 3;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_ARM_PIXELS, leftArmLEDPin, NEO_GRB + NEO_KHZ800);


uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t blue = strip.Color(0,0,255);
uint32_t black = strip.Color(0,0,0);
uint32_t white = strip.Color(255,255,255);
uint32_t light_blue = strip.Color(176,224,230);
uint32_t middle_blue = strip.Color(0,191,255);

//uint32_t rb1 = strip.Color(255,51,17);
//uint32_t rb2 = strip.Color(255,68,34);
//uint32_t rb3 = strip.Color(255,102,68);
//uint32_t rb4 = strip.Color(255,153,51);
//uint32_t rb5 = strip.Color(254,174,45);
//uint32_t rb6 = strip.Color(204,187,51);
//uint32_t rb7 = strip.Color(208,195,16);
//uint32_t rb8 = strip.Color(170,204,34);
//uint32_t rb9 = strip.Color(105,208,37);

uint32_t rb1 = strip.Color(255,51,17);
uint32_t rb2 = strip.Color(255,102,68);
uint32_t rb3 = strip.Color(254,174,45);
uint32_t rb4 = strip.Color(208,195,16);
uint32_t rb5 = strip.Color(105,208,37);
uint32_t rb6 = strip.Color(18,189,185);
uint32_t rb7 = strip.Color(68,68,221);
uint32_t rb8 = strip.Color(59,12,189);
uint32_t rb9 = strip.Color(68,34,153);

uint32_t rainbow[9] = {rb1, rb2, rb3, rb4,
                        rb5, rb6, rb7, rb8,
                        rb9};

enum ShirtMode {
  ELSA_BLAST, // 0
  RAINBOW, // 1
  CHRISTMAS, // 2
};
const int NUM_MODES = 3;

ShirtMode currentMode;

void setup() {
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
  
  pinMode( zAxisPin, INPUT );
  pinMode( yAxisPin, INPUT );
  pinMode( xAxisPin, INPUT );
  
  strip.begin();
  uint16_t n = strip.numPixels();
  strip.setBrightness(64);

  Serial.print("The number of pixels are: " ); Serial.println( n );
  switch( currentMode ) {
    case ELSA_BLAST:
      Serial.println("Performing mode: ELSA BLAST");
      break;
    case RAINBOW:
      Serial.println("Performing mode: RAINBOW");
      break;
    case CHRISTMAS:
      Serial.println("Performing mode: CHRISTMAS");
      break;
    default:
      Serial.println("FATAL: Unsupported state!");
      break;    
  }
}

void loop() {
  
  switch( currentMode ) {
    case ELSA_BLAST:
      performElsaBlast();
      break;
    case RAINBOW:
      performRainbow();
      break;
    case CHRISTMAS:
      performChristmas();
      break;
    default:
      //Serial.println("FATAL: Unsupported state!");
      break;    
  }

}

enum ElsaBlastState {
   EB_STARTING, // 0
   EB_LISTENING_FOR_MOVEMENT, // 1
   EB_ANIMATING_BLAST, // 2
};

ElsaBlastState currentElsaBlastState = EB_STARTING;

void performElsaBlast() {
   int rawX, rawY, rawZ, netX, netY, netZ;
   
   switch( currentElsaBlastState ) {
    case EB_STARTING:
      // Turn blue for two seconds with funky animation
      for ( int i=0; i<= NUM_ARM_PIXELS; i++ ) {
        strip.setPixelColor(i,blue);
        strip.show();
        delay(100);
      }
      delay(1000);
      // Turn the lights back off
      for ( int i=0; i<= NUM_ARM_PIXELS; i++ ) {
        strip.setPixelColor(i,black);
        strip.show();
        delay(100);
      }
      // Transition to listening for motion
      currentElsaBlastState = EB_LISTENING_FOR_MOVEMENT;
      break;
    case EB_LISTENING_FOR_MOVEMENT:
    
      // Listen to accelerometer for motion
      //rawX = analogRead(xAxisPin);
      //rawY = analogRead(yAxisPin);
      //rawZ = analogRead(zAxisPin);
       
      //float scaledX, scaledY, scaledZ; // Scaled values for each axis
      //scaledX = mapf(rawX, 0, 1023, -scale, scale);
      //scaledY = mapf(rawY, 0, 1023, -scale, scale);
      //scaledZ = mapf(rawZ, 0, 1023, -scale, scale);

      //Serial.print(rawX);
      //Serial.print("\t");
      //Serial.print(rawY);
      //Serial.print("\t");
      //Serial.println(rawZ);

      netX = getMotionChange( xAxisPin );
      netY = getMotionChange( yAxisPin );
      netZ = getMotionChange( zAxisPin );

      //Serial.println(netX);
      //Serial.print("\t");
      //Serial.print(netY);
      //Serial.print("\t");
      //Serial.println(netZ);

      if ( netX + netY + netZ > 400 ) {
        Serial.println(netX);
        Serial.print("\t");
        Serial.print(netY);
        Serial.print("\t");
        Serial.println(netZ);
        currentElsaBlastState = EB_ANIMATING_BLAST;
      }
     
      break;
    case EB_ANIMATING_BLAST:
      // Show cool ELSA BLAST animation
      strip.setPixelColor(0,white);
      strip.show();
      delay(100);
      strip.setPixelColor(1,white);
      strip.show();
      delay(100);
      strip.setPixelColor(2,white);
      strip.show();
      delay(100);
      strip.setPixelColor(3,light_blue);
      strip.show();
      delay(100);
      strip.setPixelColor(4,light_blue);
      strip.show();
      delay(100);
      strip.setPixelColor(5,middle_blue);
      strip.show();
      delay(100);
      strip.setPixelColor(6,middle_blue);
      strip.show();
      delay(100);
      strip.setPixelColor(7,blue);
      strip.show();
      delay(100);
      strip.setPixelColor(8,blue);
      strip.show();
      delay(3000);
       // Turn the lights back off
      for ( int i=0; i<= NUM_ARM_PIXELS; i++ ) {
        strip.setPixelColor(i,black);
        strip.show();
        delay(200);
      }
      currentElsaBlastState = EB_LISTENING_FOR_MOVEMENT;
      break;
      
    default:
      //Serial.println("FATAL: Unsupported state!");
      break;    
  }
  
}

int getMotionChange( int pin ) {
  
   unsigned int maxSample = 0;
   unsigned int minSample = 1023;
   unsigned long startMillis= millis();
   unsigned long sampleWindow = 50; // 50ms
   while (millis() - startMillis < sampleWindow)
   {
       unsigned int currentSample = analogRead(pin);
       
       if ( currentSample < 1023 ) {
       
         if ( currentSample > maxSample ) {
           maxSample = currentSample;
         }
         
         if ( currentSample < minSample ) {
           minSample = currentSample;
         }
         
       }
    }
    
    return maxSample - minSample;
}

int currentStartingRainbowPixel = 0;

void performRainbow() {
    
  for ( int i=0; i < NUM_ARM_PIXELS; i++ ) {
    
    if ( i < currentStartingRainbowPixel ) {
      // If the LED is less than the current starting pixel we need to find the element that needs to be placed in the ith position
      // This is found by taken the total number of pixels and subtracting the difference between the current starting pixel and the ith position
      strip.setPixelColor(i, rainbow[NUM_ARM_PIXELS-(currentStartingRainbowPixel-i)]); 
    } else {
      strip.setPixelColor(i, rainbow[i-currentStartingRainbowPixel]);
    }
    
    //strip.setPixelColor(i, rainbow[i]);
  }
 
  strip.show();

  if ( currentStartingRainbowPixel < NUM_ARM_PIXELS-1 ) {
     currentStartingRainbowPixel++;
  } else {
     currentStartingRainbowPixel = 0;
  }
  
  delay(50); 
  
}

void performChristmas() {
  
  for ( int i=0; i < NUM_ARM_PIXELS; i++ ) {
    strip.setPixelColor(i ,i % 2 == 0 ? red : green );
  }
  strip.show();
  delay(700);
  
  for ( int i=0; i < NUM_ARM_PIXELS; i++ ) {
    strip.setPixelColor(i ,i % 2 == 1 ? red : green );
  }
  strip.show();
  delay(700);
  
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
 

