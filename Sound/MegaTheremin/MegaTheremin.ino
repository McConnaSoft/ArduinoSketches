
#include <NewPing.h>
#include <NewTone.h>

#define RED_PIN  6
#define GREEN_PIN  10

#define BUTTON_PIN 7

#define SPEAKER_PIN 8

#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 300

int sensorValue;
int sensorLow = 1023;
int sensorHigh = 0;

int LIGHT_MODE = 1;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT );
  pinMode(GREEN_PIN, OUTPUT );

  pinMode(BUTTON_PIN, INPUT);
  
  digitalWrite( RED_PIN, LOW );
  digitalWrite( GREEN_PIN, LOW );
  
  while (millis() < 5000) {
    
    sensorValue = analogRead(A0);
    if (sensorValue > sensorHigh ) {
      sensorHigh = sensorValue;
    }
    if (sensorValue < sensorLow ) {
      sensorLow = sensorValue;
    }
    
  }
  
  digitalWrite( RED_PIN, HIGH );

}

void loop() {

  if ( !digitalRead(BUTTON_PIN) ) {
    if ( LIGHT_MODE == 1 ) {
      LIGHT_MODE = 0;
      digitalWrite( RED_PIN, LOW );
      digitalWrite( GREEN_PIN, HIGH );
    } else {
      LIGHT_MODE = 1;
      digitalWrite( RED_PIN, HIGH );
      digitalWrite( GREEN_PIN, LOW );
    }
    delay(1000);
  }

  if ( LIGHT_MODE ) {
    sensorValue = analogRead(A0);
    //int pitch = map (sensorValue, sensorLow, sensorHigh, 50, 1200);
    Serial.println( sensorValue );
    int pitch = map (sensorValue, 0, 300, 1200, 50);
    NewTone(SPEAKER_PIN,pitch,20);
    delay(10);
  } else {
    int uS = sonar.ping();
    float distance = uS / US_ROUNDTRIP_CM;
    int pitch = map (distance, 0, MAX_DISTANCE, 50, 1200);
    NewTone(SPEAKER_PIN,pitch,20);
    delay(10);
  }
  
}
