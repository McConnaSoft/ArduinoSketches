int pinR = 11; // Digital pin #0
int pinG = 10; // Digital pin #1
int pinB = 9; // Digital pin #4
int switchPin = 7; // Digital pin #7

// the setup routine runs once when you press reset:
void setup() {
Serial.begin(9600);  
pinMode(pinR, OUTPUT);
pinMode(pinG, OUTPUT);
pinMode(pinB, OUTPUT);
pinMode(switchPin, INPUT);
}

void set(int rgb[]) {
set(rgb[0], rgb[1], rgb[2]);
}

void set(int r, int g, int b) {
// using common annode LED so low means ON!
analogWrite(pinR, 255-r);
analogWrite(pinG, 255-g);
analogWrite(pinB, 255-b);
}

void loop() {
  int d = 100; 
  int switchState = digitalRead(switchPin);
  
if ( digitalRead(switchPin) == HIGH ) {  
  Serial.println("HIGH");
} else {
  Serial.println("LOW");
}  
if ( digitalRead(switchPin) == LOW ) {  
set(255, 0, 0);
delay(d);
set(128, 255, 0);
delay(d);
set(0, 255, 0);
delay(d);
set(0, 0, 255);
delay(d);
set(0, 255, 255);
delay(d);
set(255, 255, 0);
delay(d);
set(255, 0, 255);
delay(d);
set(255, 128, 0);
delay(d);
} else {
  set(255, 255, 255);
delay(d*8);
}
}
