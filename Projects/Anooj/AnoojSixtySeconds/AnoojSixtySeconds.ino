
long previousMilliseconds = 0;


void setup() {

  Serial.begin(9600);
  Serial.println("Hang tight, something will happen every 60 seconds");
  // Record the time 
  previousMilliseconds = millis();  

}



void loop() {

  Serial.begin(9600);
  doSomethingEveryTwentySeconds();
  delay(500);

}

// This function checks 
void doSomethingEveryTwentySeconds() {
  long interval = 20000;  // 20 seconds
  
  long currentMilliseconds = millis();
  // Check to see if the current time is more than 60 seconds
  // longer than the previously recorded time
  Serial.println( currentMilliseconds - previousMilliseconds );
  if ( currentMilliseconds - previousMilliseconds > interval ) {
    Serial.println("20 seconds is up, time to do work");
    // Reset the timer to the current time
    previousMilliseconds = millis();  
  }
  
}

