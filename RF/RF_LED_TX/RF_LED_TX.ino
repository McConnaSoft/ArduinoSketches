#include <VirtualWire.h>
char *controller;

const char blueON = 'B';
const char blueOFF = 'b';
const char redON = 'R';
const char redOFF = 'r';

int txPin = 12;

const int redLEDPin = 4;
const int blueLEDPin = 7;

unsigned int redState = 0;
unsigned int blueState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(redLEDPin,INPUT);
  pinMode(blueLEDPin,INPUT);
  
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(txPin);
  vw_setup(4000); // speed of data transfer Kbps
 
}

void loop(){

/*
controller="1";
vw_send((uint8_t *)controller, strlen(controller));
vw_wait_tx(); // Wait until the whole message is gone
digitalWrite(13,1);
delay(2000);
controller="0"  ;
vw_send((uint8_t *)controller, strlen(controller));
vw_wait_tx(); // Wait until the whole message is gone
digitalWrite(13,0);
delay(2000);
*/

  if ( !digitalRead( redLEDPin ) ) {
    // Red button is pressed
    if ( !redState ) {
      // debounce signal
      delay(100);
      if ( !digitalRead( redLEDPin ) ) {
        //Turn On LED
        Serial.println("Turning RED on");
        sendCommand("R");
        redState = 1;
      }
    }
  } else {
    if ( redState ) {
      Serial.println("Turning RED off");
      sendCommand("r");
      redState = 0;
    }
  }
  
  if ( !digitalRead( blueLEDPin ) ) {
    // Red button is pressed
    if ( !blueState ) {
      // debounce signal
      delay(100);
      if ( !digitalRead( blueLEDPin ) ) {
        //Turn On LED
        sendCommand("B");
        blueState = 1;
      }
    }
  } else {
    if ( blueState ) {
      sendCommand("b");
      blueState = 0;
    }
  }



}

void sendCommand(char *command) {
  vw_send((uint8_t *)command, strlen(command));
  vw_wait_tx();
  Serial.println("Done waiting");
}

