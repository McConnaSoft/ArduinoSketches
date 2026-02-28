
#include "Wire.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

const int soundPin = A1;
const int sampleWindow = 50; 


void setup() {

  Serial.begin(9600);
  matrix.begin(0x70);
  matrix.clear();
  matrix.setRotation(3);
  matrix.writeDisplay();
  
}

int mySoundReadings[8];
int currentReadingIndex = 0;

void loop() {
  
  
  matrix.clear();
    
  unsigned int sample = getSoundValue(soundPin, sampleWindow);
  Serial.println( sample );
  
  mySoundReadings[currentReadingIndex] = sample;
  
  // Draw the current bars
  drawBar( currentReadingIndex, mySoundReadings[currentReadingIndex] ); 
  
  int currentBarIndex = currentReadingIndex - 1;
  
  while ( currentBarIndex != currentReadingIndex ) {
    if ( currentBarIndex < 0 ) {
      if ( currentReadingIndex == 7 ) {
        break;
      }
      currentBarIndex = 7;
    }
    drawBar( currentBarIndex, mySoundReadings[currentBarIndex] );
    currentBarIndex--;
  }
  
  currentReadingIndex++;
  if ( currentReadingIndex == 8 ) {
      currentReadingIndex = 0;
  }

  matrix.writeDisplay();
  
}

unsigned int getSoundValue(int soundPin, int sampleWindow) 
{
  
    unsigned int maxSample = 0;
    unsigned int minSample = 1023;
    unsigned long startMillis= millis();
    while (millis() - startMillis < sampleWindow)
    {
       unsigned int currentSample = analogRead(soundPin);
       
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

void drawBar( int xValue, int soundValue ) {
  if ( soundValue >= 30 ) {
    matrix.drawRect(xValue,7,0,1, LED_GREEN);  
  } 
  
  if ( soundValue >= 35 ) {
    matrix.drawRect(xValue,6,0,1, LED_GREEN);  
  } 
  
  if ( soundValue >= 40 ) {
    matrix.drawRect(xValue,5,0,1, LED_GREEN);  
  }
  
  if ( soundValue >= 60 ) {
    matrix.drawRect(xValue,4,0,1, LED_YELLOW);  
  }
  
  if ( soundValue >= 70 ) {
    matrix.drawRect(xValue,3,0,1, LED_YELLOW);  
  } 
  
  if ( soundValue >= 80 ) {
    matrix.drawRect(xValue,2,0,1, LED_YELLOW);  
  }
  
  if ( soundValue >= 90 ) {
    matrix.drawRect(xValue,1,0,1, LED_RED);  
  } 
  
  if ( soundValue >= 125 ) {
    matrix.drawRect(xValue,0,0,1, LED_RED);  
  }
  
}

