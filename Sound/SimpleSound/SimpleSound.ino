

const int soundPin = A1;
const int soundLedPin = 13;
const int textLedPin = 12;

const int soundThreshold = 700;
const int crySoundAllowance = 5;
int crySoundCounter = 0;
int timeSinceLastCry = 0;

void setup() {

  Serial.begin(9600);
  
  pinMode( soundLedPin, OUTPUT );
  pinMode( textLedPin, OUTPUT );

  digitalWrite( soundLedPin, LOW );
  digitalWrite( textLedPin, LOW );

}


void loop() {
  int currentTime = millis();
  
  int currentSound = analogRead( soundPin );
  
  if ( currentSound <= soundThreshold ) {
      Serial.print( "Sound : " );
      Serial.println( currentSound );
      Serial.print( "Counter: " );
      Serial.println( crySoundCounter );
      digitalWrite( soundLedPin, HIGH);
      if ( currentTime - timeSinceLastCry < 10000 ) {
          // Increment the crySoundCounter
          crySoundCounter++;
          if ( crySoundCounter == crySoundAllowance ) {
            Serial.println("Sending text" );
            sendText();
          }
       } else {
         crySoundCounter = 0;
       }
       timeSinceLastCry = currentTime;
  } else {
       digitalWrite( soundLedPin, LOW);
  }
 
  delay(200);


}

void sendText() {
  crySoundCounter = 0;
  Serial.println("Turn on text light");
  digitalWrite( soundLedPin, LOW);
  digitalWrite( textLedPin, HIGH );
  delay( 5000 );
  digitalWrite( textLedPin, LOW );
}
