int pinR = 0; // Digital pin #0
int pinG = 1; // Digital pin #1
int pinB = 4; // Digital pin #4

// the setup routine runs once when you press reset:
void setup() {
pinMode(pinR, OUTPUT);
pinMode(pinG, OUTPUT);
pinMode(pinB, OUTPUT);
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
int d = 100; // delay of 100ms means the lights will change 10 times per second
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
}
