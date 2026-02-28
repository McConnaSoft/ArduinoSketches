
#define NOTE_G6  1568
#define NOTE_G2  98

const int leftAPin=2;
const int leftBPin=3;
const int leftCPin=4;
const int leftDPin=5;

const int rightAPin=6;
const int rightBPin=7;
const int rightCPin=8;
const int rightDPin=9;

const int yellowLED=17;
const int blueLED=16;
const int greenLED=15;
const int redLED=14;

const int speakerPin=18;

const int redButton=10;
const int greenButton=11;
const int blueButton=12;
const int yellowButton=13;

int lcdNumber = 0;
int sequence[100] = {0};
int buzzCount = 0;
bool flipper = false;

enum GameState {
  NEW_GAME, // 0
  SHOW_SEQUENCE, // 1
  WAITING_FOR_SEQUENCE, // 2
  CORRECT_ANSWER,  // 3
  GAME_OVER // 4
};

GameState currentState;
int currentLevel = 0;
int currentSequenceIndex = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(leftAPin, OUTPUT);  //Least Significant Bit
  pinMode(leftBPin, OUTPUT);
  pinMode(leftCPin, OUTPUT);
  pinMode(leftDPin, OUTPUT);  //Most Significant Bit

  pinMode(rightAPin, OUTPUT);  //Least Significant Bit
  pinMode(rightBPin, OUTPUT);
  pinMode(rightCPin, OUTPUT);
  pinMode(rightDPin, OUTPUT);  //Most Significant Bit

  pinMode(speakerPin, OUTPUT);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);

  pinMode(redButton, INPUT);

 // digitalWrite(redLED, HIGH);
 // digitalWrite(greenLED, HIGH);
 // digitalWrite(blueLED, HIGH);
 // digitalWrite(yellowLED, HIGH);
  
  //writeNumber( lcdNumber );
  turnOffLCDNumbers();
  currentState = NEW_GAME;
  currentLevel = 0;
  currentSequenceIndex = 0;
  randomSeed(analogRead(5));

  Serial.print( "Test: " );
  Serial.println( sequence[5] );  // Initialize random number generator
}

void loop() {

  switch ( currentState ) {
    case NEW_GAME:
      performNewGameState();
      break;
    case SHOW_SEQUENCE:
      performShowSequenceState();
      break;
    case WAITING_FOR_SEQUENCE:
      performWaitingForSequenceState();
      break;
    case CORRECT_ANSWER:
      performCorrectAnswerState();
      break;
    case GAME_OVER:
      performGameOverState();
      break;  
    default:
      Serial.println("FATAL: Unsupported state!");
      break;
  }
  
}

void performNewGameState() {
  
  currentLevel = 0;
  int counter = 0;
  writeNumber( 0 );
  
  while( digitalRead( redButton ) == HIGH ) {
     
    delay(100);
    digitalWrite( redLED, HIGH ); 
    counter++;
    if ( counter == 5 ) {
       
      counter = 0;
      if ( flipper ) {
         turnOffLCDNumbers();
         flipper = false;
      } else {
         writeNumber( 0 );
        flipper = true;
      }
      
    }
  
  }
  allLED(HIGH);
  delay(1000);
  allLED(LOW);
  delay(1000);
  currentSequenceIndex = 0;
  increaseLevel();
}

void performShowSequenceState() {
  writeNumber(currentLevel);
  if ( currentSequenceIndex < currentLevel ) {
    changeLED( sequence[currentSequenceIndex], HIGH );
    delay(400);
    changeLED( sequence[currentSequenceIndex], LOW );
    delay(400);
    currentSequenceIndex++;
  } else {
    currentSequenceIndex = 0;
    currentState = WAITING_FOR_SEQUENCE;
  }
  
}

void performWaitingForSequenceState() {
  
  int red, green, blue, yellow;
  // Wait for any one of the buttons to be pressed
  do {
      red = digitalRead( redButton ); 
      green = digitalRead( greenButton );
      blue = digitalRead( blueButton );
      yellow = digitalRead( yellowButton );
  } while ( red && green && blue && yellow );
  
  int currentButtonIndex = 0;
  if ( !red ) {
    currentButtonIndex = 0;
  } else if ( !green ) {
    currentButtonIndex = 1; 
  } else if ( !blue ) {
    currentButtonIndex = 2; 
  } else if ( !yellow ) {
    currentButtonIndex = 3; 
  }
  Serial.println( "Waiting for depress" );
  // Wait for button press to stop
  while ( !(red && green && blue && yellow) ) {
    red = digitalRead( redButton ); 
    green = digitalRead( greenButton );
    blue = digitalRead( blueButton );
    yellow = digitalRead( yellowButton );
    delay(100);
  }   
  
  
  Serial.print( "Checking index: " );
  Serial.println( currentSequenceIndex );
  Serial.print( "Button index: " );
  Serial.println( currentButtonIndex );
  Serial.print( "Sequence value: " );
  Serial.println( sequence[currentSequenceIndex] );
  Serial.print( "Current Level: " );
  Serial.println( currentLevel );
  if ( sequence[currentSequenceIndex] == currentButtonIndex ) {
    // Correct match!
    playHappyNote();  
    if ( (currentSequenceIndex + 1) == currentLevel ) {
      Serial.println("Correct Answer");
      currentState = CORRECT_ANSWER;
    } else {
      Serial.println("Right so far");
      currentSequenceIndex++;
      return;
    }
    
  } else {
     buzzCount = 0;
     currentState = GAME_OVER;
  }

}

void performCorrectAnswerState() {
    // Do some happy beeping
    for ( int i= 0; i<3; i++ ) {
      allLED( HIGH);
      playHappyNote();
      delay(200);
      allLED(LOW);
      delay(200);
    }
    increaseLevel();  
}



void performGameOverState() {
  int counter = 0;
  
  while ( digitalRead(redButton) == HIGH ) {
    delay(100);

    if ( buzzCount < 3 ) {
      playSadNote();
      delay(300);
    }
    buzzCount++;
    counter++;
    if ( counter == 5 ) { 
      digitalWrite( redLED, HIGH ); 
      counter = 0;
      if ( flipper ) {
          turnOffLCDNumbers();
          flipper = false;
      } else {
          writeNumber( currentLevel );
          flipper = true;
      }
    }
  }
  digitalWrite( redLED, LOW ); 
  currentState = NEW_GAME;
}

void increaseLevel() {
  currentLevel++;
  sequence[currentLevel-1] = (int)random(4);
  currentSequenceIndex = 0;
  currentState = SHOW_SEQUENCE;
}

/*
 * Utility Functions
 */

// Sound Functions
void playHappyNote() {
  tone( speakerPin, 1000  , 100);
}

void playSadNote() {
  tone( speakerPin, 100, 100 );
}

// LED Functions
void allLED( int value ) {
  digitalWrite( redLED, value );
  digitalWrite( greenLED, value );
  digitalWrite( blueLED, value );
  digitalWrite( yellowLED, value );
}

void changeLED( int index, int value ) {
  switch ( index ) {
    case 0:
      digitalWrite(redLED, value);
      break;
    case 1:
      digitalWrite(greenLED, value);
      break;
    case 2:
      digitalWrite(blueLED, value);
      break;
    case 3:
      digitalWrite(yellowLED, value);
      break;      
    default:
      break;  
  }
}
 

// 7-Segment Convenience functions

void incrementLCDNumber() {
  lcdNumber++;
  if ( lcdNumber > 99 ) {
    lcdNumber = 0;
  }
}

void turnOffLCDNumbers() {
  writeLeftLCDNumber( 15 );
  writeRightLCDNumber( 15 );
}


void writeNumber( int num ) {
  
  if ( num > 99 || num < 0 ) {
    Serial.println( "Invalid number..." ); 
    return;
  }

  writeLeftLCDNumber( num / 10 );
  writeRightLCDNumber( num % 10 );
  
}

void writeLeftLCDNumber( int num ) {
  //Serial.println("Writing left number...");
  writeLCDNumber( num, leftAPin, leftBPin, leftCPin, leftDPin ); 
}

void writeRightLCDNumber( int num ) {
  //Serial.println("Writing right number...");
  writeLCDNumber( num, rightAPin, rightBPin, rightCPin, rightDPin );
}

void writeLCDNumber( int num, int aPin, int bPin, int cPin, int dPin ) {
 if ( num > 10 ) {
    //Serial.println( "Invalid number..." ); 
    //return;   
 }
  
 if ( num % 2 == 1 ) {
    //Serial.print("1");
    digitalWrite(aPin, HIGH);
 } else {
    //Serial.print("0");
    digitalWrite(aPin, LOW);
 }
 if ( num % 4 >= 2 ) {
    //Serial.print("1");
    digitalWrite(bPin, HIGH);
 } else {
    //Serial.print("0");
    digitalWrite(bPin, LOW);
 }
 if ( num % 8 >= 4 ) {
    //Serial.print("1");
    digitalWrite(cPin, HIGH);
 } else {
    //Serial.print("0");
    digitalWrite(cPin, LOW);
 }
 if ( num % 16 >= 8 ) {
    //Serial.print("1");
    digitalWrite(dPin, HIGH);
 } else {
    //Serial.print("0");
    digitalWrite(dPin, LOW);
 }
 //Serial.println("");

}




