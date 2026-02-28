#include <SPI.h>
#include <nRF24L01.h>
#include <rf24.h>

#define buttonPin 2

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

int buttonState = LOW;         // Current button state
int lastButtonState = LOW;     // Previous button state

unsigned long lastDebounceTime = 0; // Timestamp of last debounce check
unsigned long debounceDelay = 50;   // Debounce time in milliseconds

unsigned long lastRadioCheck = 0;     // Last time we checked the connection
unsigned long checkInterval = 5000; // Check every 5 seconds

const char buttonMessage[] = "A"; // Let's keep it to a single character

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  if (!initializeRadio()) {
    Serial.println("Failed to initialize radio. Check connections!");
  } else {
    Serial.println("Radio initialized successfully!");
  }

}

void loop() {
  
  debouncedButtonCheck();

  // Check and reconnect if needed
  //checkRadioConnection();

}

bool initializeRadio() {
    
    radio.begin();    
    radio.openWritingPipe(addresses[1]); // 00001
    radio.openReadingPipe(1, addresses[0]); // 00002     
    radio.setPALevel(RF24_PA_MAX);
   // radio.setDataRate(RF24_250KBPS);        // Set data rate
   // radio.setChannel(90);                 // Set RF channel
  //  radio.setRetries(3, 15);
    radio.stopListening();    // Set to transmit mode
    return true;

}


void doButtonAction() {
    Serial.println("The button was pressed");
    radio.write(&buttonMessage, sizeof(buttonMessage));
}

void debouncedButtonCheck() {

  // Read the state of the button
  int reading = digitalRead(buttonPin);

  // Check if the button state has changed
  if (reading != lastButtonState) {
    // Reset the debounce timer
    lastDebounceTime = millis();
  }

  // Check if debounce time has passed
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has stabilized, update the button state
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        doButtonAction();
      }
    }
  }

  // Save the reading for the next loop iteration
  lastButtonState = reading;

}

// Function to check radio status and reconnect if needed
void checkRadioConnection() {
  if (millis() - lastRadioCheck >= checkInterval) {
    lastRadioCheck = millis(); // Update the timestamp

    if ( !radio.isValid() ) {                    // Carrier test failed (possible lockup)
      Serial.println("Radio not responding. Reinitializing...");
      initializeRadio(); // Reinitialize the radio
    } else {
      Serial.println("Radio is responsive.");
    }
  }
}
