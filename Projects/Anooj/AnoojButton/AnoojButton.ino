/* Melody
 * (cleft) 2005 D. Cuartielles for K3
 *
 * This example uses a piezo speaker to play melodies.  It sends
 * a square wave of the appropriate frequency to the piezo, generating
 * the corresponding tone.
 *
 * The calculation of the tones is made following the mathematical
 * operation:
 *
 *       timeHigh = period / 2 = 1 / (2 * toneFrequency)
 *
 * where the different tones are described as in the table:
 *
 * note   frequency   period  timeHigh
 * c          261 Hz          3830  1915  
 * d          294 Hz          3400  1700  
 * e          329 Hz          3038  1519  
 * f          349 Hz          2864  1432  
 * g          392 Hz          2550  1275  
 * a          440 Hz          2272  1136  
 * b          493 Hz          2028  1014  
 * C          523 Hz          1912  956
 *
 * http://www.arduino.cc/en/Tutorial/Melody
 */
  
int speakerPin = 9;
int buttonPin = 2; //pushbutton
int val = 0;

int length = 15; // the number of notes
//char notes[] = "ccggaagffeeddc "; // a space represents a rest
char notes[] = "ggggggggggggggg";

//int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int beats[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int tempo = 100;

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void setup() {
  pinMode(speakerPin, OUTPUT);
  pinMode(buttonPin, INPUT); //declare pushbutton as input
}

// Notice that this is a global variable that keeps track if the program should play the notes.
// The value of the variable will be changed each time the user presses the hidden button.
bool playNotes = false;

void loop() {
  val = digitalRead(buttonPin);
  if (val == LOW)
  {
    // The button has been pressed switch the value of playNotes
    playNotes = !playNotes;
    // Wait until the user lets go of the button.
    // This while loop will check every 200 milliseconds to see if the button is still pressed.  
    // One the user lets go of the button, the value will be HIGH and code will exit the while loop
    // and continue.
    while ( digitalRead(buttonPin) == LOW ) {
      delay(200);
    }
  }

  // Play the notes if playNotes is set to true
  if ( playNotes ) {
    
      for (int i = 0; i < length; i++) {
        if (notes[i] == ' ') {
          delay(beats[i] * tempo); // rest
        } else {
          playNote(notes[i], beats[i] * tempo);
        }
    
        // pause between notes
        delay(tempo / 2); 
     }
  }
  
}
