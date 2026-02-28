#include "Keypad.h"
#include <Servo.h> 
#include <EEPROM.h> 
 
Servo servo;  

const int KEY_SIZE = 4;
 
const int SERVO_PIN = 11;
const int BUZZER_PIN = 13;
const int RED_LED = 2;
const int GREEN_LED = 12;

const int lockedPos = 28;
const int unlockedPos = 125;

const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char keys[ROWS][COLS] =
{
{'1','2','3' },
{'4','5','6' },
{'7','8','9' },
{'*','0','#' }
};
byte pin_rows[ROWS] = {3, 4, 5, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLS] = {7, 8, 9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROWS, COLS );

char KEY[KEY_SIZE] = {'3','0','1','0'};  // default secret key
char attempt[KEY_SIZE] = {'0','0','0','0'};
int keyCounter=0;

enum Days{ LOCKED = 1, UNLOCKED = 2 };

Days doorState = LOCKED;

void setup()
{
   Serial.begin(9600);
   pinMode(BUZZER_PIN, OUTPUT); // Set buzzer - pin 9 as an output
   pinMode(GREEN_LED,OUTPUT);
   pinMode(RED_LED,OUTPUT);
   servo.attach(SERVO_PIN);

   digitalWrite( GREEN_LED, LOW );
   digitalWrite( RED_LED, HIGH );
   doorState = EEPROMReadInt(0); 
   updateServo();
   reportDoorState(); 
   
   Serial.println("Here we go...");
     

}
void correctKey() // do this if the correct KEY is entered
{
   Serial.println("KEY ACCEPTED...");
   digitalWrite( GREEN_LED, HIGH );
   playTone( 2000 );
   unlockDoor();
   digitalWrite( GREEN_LED, LOW );
   clearKeyMemory();
    
   
}
void incorrectKey() // do this if an incorrect KEY is entered
{
   Serial.println("KEY REJECTED!");
   playTone( 500 ); 
   for ( int i = 0; i < 5; i ++ ) {
     digitalWrite( RED_LED, LOW );
     delay(250);
     digitalWrite( RED_LED, HIGH );
     delay(250);
   }
   clearKeyMemory();
}

void checkKEY()
{
   int correct=0;
   int i;
   for ( i = 0; i < KEY_SIZE ; i++ ) {
      if (attempt[i]==KEY[i]) {
         correct++;
      }
   }
   
   if (correct==KEY_SIZE && keyCounter==KEY_SIZE) {
      correctKey();
   } else {
      incorrectKey();
   }
   
}

void readKeypad()
{
   char key = keypad.getKey();
   if (key)
   {
    Serial.println( key );
      
      switch(key)
      {
      case '*':
         if ( doorState == UNLOCKED ) {
            lockDoor();
            clearKeyMemory();
         }
         break;
      case '#':
         if ( doorState == LOCKED ) {
            delay(100); // added debounce
            checkKEY();
         }
         break;
      default:
         //if ( keyCounter==KEY_SIZE ) {
         //   incorrectKey();
         //} else {
            playTone( 1000, 200 );
            if ( keyCounter < KEY_SIZE ) {
              attempt[keyCounter]=key;
              keyCounter++;
            }
         //}
      }
      
  }
}
void loop()
{
   readKeypad();
}

void playTone( int toneValue ) {
  tone(BUZZER_PIN, toneValue); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(BUZZER_PIN);     // Stop sound...
}

void playTone( int toneValue, int duration ) {
  tone(BUZZER_PIN, toneValue); // Send 1KHz sound signal...
  delay(duration);        // ...for 1 sec
  noTone(BUZZER_PIN);     // Stop sound...
}

void unlockDoor() {
  servo.write(unlockedPos);
  doorState = UNLOCKED;
  EEPROMWriteInt(0,doorState);
}

void lockDoor() {
  servo.write(lockedPos);
  doorState = LOCKED;
  EEPROMWriteInt(0,doorState);
}

void updateServo() {
  switch( doorState ) {
    case LOCKED:
       servo.write(lockedPos);
       break;
    case UNLOCKED:
       servo.write(unlockedPos);
       break;    
  }
}

void clearKeyMemory() {
  //for (int zz=0; zz<KEY_SIZE; zz++) // clear previous key input
  // {
  //    attempt[zz]=0;
  // }
   memset(attempt, 0, KEY_SIZE*sizeof(attempt[0])); 
   keyCounter=0;
}

void reportDoorState() {
    switch ( doorState ) {
      case LOCKED:
        Serial.println("The door is locked");
        break;
      case UNLOCKED:
        Serial.println("The door is unlocked");
        break;
      default:
        Serial.println("The door is in an unknown state");
        break;  
    }
}

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
