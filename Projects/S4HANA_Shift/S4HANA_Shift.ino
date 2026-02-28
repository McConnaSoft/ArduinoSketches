
enum BoardState {
  GETTING_ANSWER, // 0
  FLASHING_ANSWER, // 1
  SHOWING_CODE // 2
};

BoardState currentState;

const int SER_Pin = 2;   //pin 14 on the 75HC595
const int SRCLK_Pin = 3; //pin 11 on the 75HC595
const int RCLK_Pin = 4;  //pin 12 on the 75HC595

const int sevenSeg_6_Pin = 8;
const int sevenSeg_5_Pin = 7;
const int sevenSeg_4_Pin = 6;
const int sevenSeg_3_Pin = 5;
const int sevenSeg_2_Pin = 9;
const int sevenSeg_1_Pin = 10;

const int lcdPins[6] = { sevenSeg_1_Pin, sevenSeg_2_Pin, sevenSeg_3_Pin, sevenSeg_4_Pin, sevenSeg_5_Pin, sevenSeg_6_Pin };

// My 7-segment LEDs go by:
// dp,a,b,c,d,e,f,g

const int letterS[8] = {1,0,1,0,0,1,0,0};
const int letterH[8] = {1,1,0,0,1,0,0,0};
const int letterA[8] = {1,0,0,0,1,0,0,0};
const int letterN[8] = {1,0,0,0,1,0,0,1};
const int smallLetterN[8] = {1,1,1,0,1,0,1,0};
const int number1[8] = {1,1,0,0,1,1,1,1};
const int number2[8] = {1,0,0,1,0,0,1,0};
const int number3[8] = {1,0,0,0,0,1,1,0};
const int number4[8] = {1,1,0,0,1,1,0,0};
const int number5[8] = {1,0,1,0,0,1,0,0};
const int number6[8] = {1,0,1,0,0,0,0,0};
const int number7[8] = {1,0,0,0,1,1,1,1};
const int number8[8] = {1,0,0,0,0,0,0,0};
const int number9[8] = {1,0,0,0,1,1,0,0};
const int number0[8] = {1,0,0,0,0,0,0,1};

const char* sevenSegCharacters= "SHAN4";
int sevenSegPosition[6] = {4,2,3,1,0,4};

//How many of the shift registers - change this
#define number_of_74hc595s 6  

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[numOfRegisterPins];

void setup(){
  Serial.begin(9600);
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);

  pinMode(sevenSeg_1_Pin, INPUT_PULLUP);
  pinMode(sevenSeg_2_Pin, INPUT_PULLUP);
  pinMode(sevenSeg_3_Pin, INPUT_PULLUP);
  pinMode(sevenSeg_4_Pin, INPUT_PULLUP);
  pinMode(sevenSeg_5_Pin, INPUT_PULLUP);
  pinMode(sevenSeg_6_Pin, INPUT_PULLUP);

  updateCharacters();
  currentState = GETTING_ANSWER;
  
}               


//set all register pins to LOW
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = HIGH;
  }
} 


//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters(){

  digitalWrite(RCLK_Pin, LOW);
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);
    int val = registers[i];
    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);
  }
  digitalWrite(RCLK_Pin, HIGH);

}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}

void incrementSegmentPosition( int index ) {
  if ( sevenSegPosition[index] >= 4 ) {
    sevenSegPosition[index] = 0;
  } else {
    sevenSegPosition[index]++;
  }
}

long currentCode = 0L;
void loop(){
  
  switch ( currentState ) {
    case GETTING_ANSWER:
      currentCode = 0L;
      for ( int i=0; i < 6; i++ ) {
        buttonCheck( i, lcdPins[i] ); 
      }
      break;
    case FLASHING_ANSWER:
    
      for ( int j=0; j < 4; j++ ) {
        Serial.println("Here!!!"); 
        clearRegisters();
        writeRegisters();
        delay(500);
        updateCharacters();
        delay(500);  
      }
      currentState = SHOWING_CODE;
      break;
    case SHOWING_CODE:
      clearRegisters();
      writeRegisters();
      delay(500);
      displayCode();
      delay(500); 
      break;
    default:
      Serial.println("FATAL: Unsupported state!");
      break;
  }
  
  

}

void buttonCheck( int index, int buttonPin ) {
  if ( !digitalRead( buttonPin ) ) {
    delay(50);  // Debouncing signal
    if ( !digitalRead( buttonPin ) ) {
       // Button is pressed
       Serial.println("Pressed!!!!");
       incrementSegmentPosition( index );
       updateCharacters();
       if ( sevenSegPosition[0] == 0 && 
           sevenSegPosition[1] == 4 &&
           sevenSegPosition[2] == 1 &&
           sevenSegPosition[3] == 2 &&
           sevenSegPosition[4] == 3 &&
           sevenSegPosition[5] == 2
         ) {
        Serial.println("Correct!!!");  
        currentState = FLASHING_ANSWER;
      }
      // Wait for button to be released
      while ( !digitalRead( buttonPin ) ) {
        // No operation
      }
    }
  }
}

void updateCharacters() {
  
  clearRegisters();

  //setRegisterPin( 1, 0 );
  
  writeCharacter( 0, sevenSegCharacters[ sevenSegPosition[0] ] );
  writeCharacter( 1, sevenSegCharacters[ sevenSegPosition[1] ] );
  writeCharacter( 2, sevenSegCharacters[ sevenSegPosition[2] ] );
  writeCharacter( 3, sevenSegCharacters[ sevenSegPosition[3] ] );
  writeCharacter( 4, sevenSegCharacters[ sevenSegPosition[4] ] );
  writeCharacter( 5, sevenSegCharacters[ sevenSegPosition[5] ] );
  
  writeRegisters();  //MUST BE CALLED TO DISPLAY CHANGES

}

void writeCharacter( int registerIndex,char character) {
   
   switch ( character ) {
    case 'S':
      writeCharacterByArray( registerIndex, letterS );
      break;
    case 'H':
      writeCharacterByArray( registerIndex, letterH );
      break;
    case 'A':
      writeCharacterByArray( registerIndex, letterA );
      break;
    case 'N':
      writeCharacterByArray( registerIndex, letterN );
      break;
    case '4':
      writeCharacterByArray( registerIndex, number4 );
      break;          
    default:
      Serial.println("Unsupported character");    
   }
}

void writeCharacterByArray( int registerIndex, int characterArray[] ) {
  int offset = registerIndex * 8;
  for(int i = 0; i < 8; i++){
      setRegisterPin( offset + i, characterArray[i] ); 
  }
}

void displayCode() {
   clearRegisters();
   writeCharacterByArray( 0, number1 ); 
   writeCharacterByArray( 1, number2 );
   writeCharacterByArray( 2, number3 );
   writeRegisters();
}





