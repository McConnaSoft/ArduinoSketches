
#define HAND_DETECT 7
#define LED 8

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode( HAND_DETECT, INPUT );
  pinMode( LED, OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( digitalRead(HAND_DETECT) ) {
    digitalWrite( LED, HIGH );
  } else {
    Serial.println("LOW");
    digitalWrite(LED, LOW );
  }
  delay(100);
}
