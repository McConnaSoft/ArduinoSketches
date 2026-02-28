#include <SPI.h>
#include <nRF24L01.h>
#include <rf24.h>
#include <string.h>  // Include the string functions library


#define ledPin 2

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

int buttonState = LOW;         // Current button state
int lastButtonState = LOW;     // Previous button state

unsigned long lastDebounceTime = 0; // Timestamp of last debounce check
unsigned long debounceDelay = 50;   // Debounce time in milliseconds

unsigned long lastRadioCheck = 0;     // Last time we checked the connection
unsigned long checkInterval = 5000; // Check every 5 seconds

const char buttonMessage[1] = "A"; // Let's keep it to a single character

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  
  if (!initializeRadio()) {
    Serial.println("Failed to initialize radio. Check connections!");
  } else {
    Serial.println("Radio initialized successfully!");
  }

}

void loop() {
  
  if (radio.available()) { // if nrf has any incoming data
    performRadioRead();
  }
  // Check and reconnect if needed
  //checkRadioConnection();

}

void performRadioRead() {
  char incoming[1] = "";
    radio.read(&incoming, sizeof(incoming));
    Serial.print("We have read: ");
    Serial.println(incoming);

    if ( strcmp(incoming, "A") == 0 ) {
      Serial.println("Triggering pager");
      digitalWrite( ledPin, HIGH);
      delay(1000);
      digitalWrite( ledPin, LOW);
    }
}

bool initializeRadio() {
    
    radio.begin();    
    radio.openWritingPipe(addresses[1]); // 00001
    radio.openReadingPipe(1, addresses[0]); // 00002     
    radio.setPALevel(RF24_PA_MAX);
    //radio.setDataRate(RF24_250KBPS);        // Set data rate
    //radio.setChannel(90);                 // Set RF channel
    //radio.setRetries(3, 15);
    radio.startListening();    
    return true;

}

// Function to check radio status and reconnect if needed
void checkRadioConnection() {
  if (millis() - lastRadioCheck >= checkInterval) {
    lastRadioCheck = millis(); // Update the timestamp

    if ( !radio.isValid() ) {                    
      Serial.println("Radio not responding. Reinitializing...");
      initializeRadio(); // Reinitialize the radio
    } else {
      Serial.println("Radio is responsive.");
    }
  }
}

