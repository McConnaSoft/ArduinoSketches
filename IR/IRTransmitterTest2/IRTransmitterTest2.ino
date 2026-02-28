#include <IRremote.h>

#define YELLOW_BUTTON_PIN 4

IRsend irsend;
 
void setup()
{
  Serial.begin(9600);
  pinMode( YELLOW_BUTTON_PIN, INPUT );
}
 
void loop() {
  if ( !digitalRead( YELLOW_BUTTON_PIN ) ) {
    delay(50);  // Double check to debounce the button press
    if ( !digitalRead(YELLOW_BUTTON_PIN) ) {
      //send a code every time a character is received from the serial port
      //Sony DVD power A8BCA
      Serial.println("Sending...");
      irsend.sendSony(0xa8bca, 20);
      delay(500);
    }
  }
}
