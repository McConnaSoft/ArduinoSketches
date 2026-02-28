#include <IRLib.h>
 
IRsend My_Sender;

#define YELLOW_BUTTON_PIN 4
#define RED_BUTTON_PIN 5
#define GREEN_BUTTON_PIN 6
#define BLUE_BUTTON_PIN 7

 
void setup()
{
  Serial.begin(9600);
  pinMode( YELLOW_BUTTON_PIN, INPUT );
  pinMode( RED_BUTTON_PIN, INPUT );
  pinMode( GREEN_BUTTON_PIN, INPUT );
  pinMode( BLUE_BUTTON_PIN, INPUT );
}
 
void loop() {
  if ( !digitalRead( YELLOW_BUTTON_PIN ) ) {
      My_Sender.send(SONY,0x11111, 20);
  } else if ( !digitalRead( RED_BUTTON_PIN ) ) {
      My_Sender.send(SONY,0x22222, 20);
  } else if ( !digitalRead( GREEN_BUTTON_PIN ) ) {
      My_Sender.send(SONY,0x33333, 20);
  } else if ( !digitalRead( BLUE_BUTTON_PIN ) ) {
      My_Sender.send(SONY,0x44444, 20);
  }
  

  
}
