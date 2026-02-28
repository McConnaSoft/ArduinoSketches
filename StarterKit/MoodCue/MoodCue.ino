

#include <Servo.h>

Servo myServo;

int const potPin = A0;
int potVal;
int angle;
int currentAngle;

void setup() {
  myServo.attach(9);   // Attaches servo to pin 9
  currentAngle = 0;
  Serial.begin(9600);
}

void loop() {
  potVal = analogRead(potPin);
  
  
  angle = map(potVal, 0, 1023, 0, 179);  // Maps potval from 0-1023 to 0-179
  
  if ( angle != currentAngle ) {
    Serial.print("potVal: ");
    Serial.println(potVal);
    Serial.print("angle: ");
    Serial.println(angle);
    myServo.write(angle);
    currentAngle = angle;
  }
  
  delay(15);
}
