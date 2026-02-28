#include <NewPing.h>
#include <EEPROM.h>
 
#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 300
#define LED_PIN 13
#define BUTTON_PIN 4

const int STARTING_DISTANCE = 130;
const int STOPPING_DISTANCE = 50;
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
  pinMode( LED_PIN, OUTPUT );
  pinMode( BUTTON_PIN, INPUT_PULLUP );

  digitalWrite( LED_PIN, LOW );

  Serial.print( "The last read was: " );
  Serial.print( EEPROM.read( 0 ) );
  Serial.println( "cm." );
  
}
 
void loop() {
  
  delay(200);
  int uS = sonar.ping();
  float distance = uS / US_ROUNDTRIP_CM;
  Serial.print("Ping: ");
  Serial.print( distance );
  Serial.println("cm");
  
  if ( !digitalRead(BUTTON_PIN) ) 
  {
    EEPROM.write(0,distance);
  }
  
  if ( distance <= STARTING_DISTANCE && distance > STOPPING_DISTANCE ) 
  {
    digitalWrite(LED_PIN, HIGH);  
  } else {
    digitalWrite(LED_PIN, LOW);  
  } 
   
  
}
