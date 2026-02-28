
#include <Servo.h>

#define LED_PIN 5
#define SERVO_PIN A0
#define SOUND_PIN A2

Servo flowerServo;

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)

void setup() {
  
  // put your setup code here, to run once:
 // pinMode( LED_PIN, OUTPUT );

  flowerServo.attach( SERVO_PIN );

  Serial.begin( 9600 );
  Serial.println( "Dancing Flower Example" );

  flowerServo.write(90); // Starting point for flower
   
   
}

void loop() {


  
  int val = getSoundValue(SOUND_PIN, sampleWindow);
  Serial.println( val ); 
  if ( val > 700 ) {
    dance();
    delay(1000);
  }
  
}

void dance() {
   danceStep();
   delay(50);
   danceStep();
   delay(100);
   danceStep();
}


void danceStep()
{
  flowerServo.write( 88 );
  delay( 100 );
  flowerServo.write( 92 );
  delay( 100 );
  flowerServo.write( 85 );
  delay( 200 );
  flowerServo.write( 95 );
  delay( 200 );
  flowerServo.write( 90 ); 
}

unsigned int getSoundValue(int soundPin, int sampleWindow) 
{
  
    unsigned int maxSample = 0;
    unsigned int minSample = 1023;
    unsigned long startMillis= millis();
    while (millis() - startMillis < sampleWindow)
    {
       unsigned int currentSample = analogRead(soundPin);
       //Serial.println( currentSample );
       if ( currentSample < 1023 ) {
       
         if ( currentSample > maxSample ) {
           maxSample = currentSample;
         }
         
         if ( currentSample < minSample ) {
           minSample = currentSample;
         }
         
       }
    }
    if ( maxSample == 0 ) {
      minSample = 0;
    }
    return maxSample - minSample;
} 

