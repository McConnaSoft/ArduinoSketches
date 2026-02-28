#define RED_PIN 11
#define GREEN_PIN 10
#define BLUE_PIN 9

#define RED2_PIN 6
#define GREEN2_PIN 5
#define BLUE2_PIN 3

#define RED_POT_PIN A0
#define GREEN_POT_PIN A1
#define BLUE_POT_PIN A2

#define BUTTON_PIN 8


byte voiceVal = 0;

void setup() {

  Serial.begin(9600);

  analogWrite( RED_PIN, 255 );
  analogWrite( GREEN_PIN, 255 );
  analogWrite( BLUE_PIN, 255 );

  pinMode( RED_PIN, OUTPUT );
  pinMode( GREEN_PIN, OUTPUT );
  pinMode( BLUE_PIN, OUTPUT );

  pinMode( BUTTON_PIN, INPUT );
    
  delay(2000);
  // Switch voice recognition to compact mode
  //voiceRec
  Serial.write(0xAA);
  //voiceRec
  Serial.write(0x37);
 
  delay(1000);

  // Import group 1 into the voice recognition module
  //voiceRec
  Serial.write(0xAA);
  //voiceRec
  Serial.write(0x21);

  Serial.println("Here we go");


  
}

void loop() {
   
  while ( Serial.available() ) {
    voiceVal = Serial.read();
    switch (voiceVal) 
    {
        case 0x11:
          Serial.println("Blue was spoken!");
          writeBluePWM();
          break;
        case 0x12:
          Serial.println("Red was spoken!");
          writeRedPWM();
          break;
        case 0x13:
          Serial.println("Green was spoken!");
          writeGreenPWM();
          break;
        case 0x14:
          Serial.println("Pink was spoken!");
          writePinkPWM();
          break;
        case 0x15:
          Serial.println("Yellow was spoken!");
          writeYellowPWM();
          break;
    }   
  }

  analogWrite( RED2_PIN, map( analogRead( RED_POT_PIN ), 0, 1024, 255, 0 ) );
  analogWrite( GREEN2_PIN, map( analogRead( GREEN_POT_PIN ), 0, 1024, 255, 0 ) );
  analogWrite( BLUE2_PIN, map( analogRead( BLUE_POT_PIN ), 0, 1024, 255, 0 ) );

  if ( !digitalRead( BUTTON_PIN ) ) {
      analogWrite( RED_PIN, 255 );
      analogWrite( GREEN_PIN, 255 );
      analogWrite( BLUE_PIN, 255 );
  }
  
  delay(50);
 
}

void writeRedPWM() {
  analogWrite( RED_PIN, 0 );
  analogWrite( GREEN_PIN, 255 );
  analogWrite( BLUE_PIN, 255 );
}

void writeGreenPWM() {
  analogWrite( RED_PIN, 255 );
  analogWrite( GREEN_PIN, 0 );
  analogWrite( BLUE_PIN, 255 );
}

void writeBluePWM() {
  analogWrite( RED_PIN, 255 );
  analogWrite( GREEN_PIN, 255 );
  analogWrite( BLUE_PIN, 0 );
}

void writePinkPWM() {
  analogWrite( RED_PIN, 0 );
  analogWrite( GREEN_PIN, 204 );
  analogWrite( BLUE_PIN, 200 );
}

void writeYellowPWM() {
  analogWrite( RED_PIN, 0 );
  analogWrite( GREEN_PIN, 0 );
  analogWrite( BLUE_PIN, 255 );
}



