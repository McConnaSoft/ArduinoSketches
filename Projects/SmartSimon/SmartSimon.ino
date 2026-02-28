#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "McConnellWifi"          
#define WLAN_PASS       "Frassati1974"
//#define WLAN_SSID       "Timothy's iPhone"           // cannot be longer than 32 characters!
//#define WLAN_PASS       "TimmerMobile"

// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

// What page to grab!
#define WEBSITE      "api.justyo.co"
#define WEBPAGE      "/yo"

const int redLedPin = 8;
const int greenLedPin = 9;

const int yellowLedPin = 7;
const int blueLedPin = 6;

const int switchPin = 2;
const int potPin = A0;
const int soundPin = A1;

unsigned long timeSinceLastCry = 0;
int textInterval = 300000; // Five minutes
int sensitivityRating = 0;
int previousSensitivityRating = 0;

uint32_t ip;

void setup(void)
{
  Serial.begin(115200);

  pinMode( redLedPin, OUTPUT );
  pinMode( greenLedPin, OUTPUT );
  pinMode( yellowLedPin, OUTPUT );
  pinMode( blueLedPin, OUTPUT );
 
  digitalWrite( redLedPin, LOW );
  digitalWrite( greenLedPin, LOW );
  digitalWrite( yellowLedPin, LOW );
  digitalWrite( blueLedPin, LOW );
 
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    digitalWrite( redLedPin, HIGH);
    while(1);
  }
  Serial.println("Wifi Driver Initailized!");
 
 
  for( int i=0; i<4; i++ ) {
  digitalWrite( greenLedPin, HIGH );
  delay(300);
  digitalWrite( greenLedPin, LOW );
  }
 
}

void loop(void)
{
  
  //performSoundCheck();
  //delay(50);
  //return;
  
  unsigned long currentTime = millis();
  checkSensitivity();
  int switchState = digitalRead( switchPin );
 
  if ( switchState == HIGH ) {
      previousSensitivityRating = sensitivityRating;
   
      if ( !cc3000.checkConnected()  ) {
        getWifiConnection();
        return;
      }
      if ( timeSinceLastCry == 0 || abs(currentTime - timeSinceLastCry ) > 60000 ) {
        digitalWrite( yellowLedPin, LOW);
        performSoundCheck();
      }
      return;  
  } else {
      ensureDisconnect();
      if (previousSensitivityRating != sensitivityRating )
      {
        displaySensitivityIndictor(sensitivityRating);
        previousSensitivityRating = sensitivityRating;
      }
  }
  delay(250);
 
}

void checkSensitivity() {
  int sensitivitySignal = analogRead( potPin );
  sensitivityRating = map( sensitivitySignal,0,1024,5,0);
  //Serial.println( sensitivitySignal );
  //Serial.println( sensitivityRating );
}

void displaySensitivityIndictor(int rating )
{
  for (int i=0; i < rating; i++ ) {
    digitalWrite( redLedPin, HIGH );
    delay(200);
    digitalWrite( redLedPin, LOW );
    delay(200);
  }
}


void getWifiConnection() {
    // Attempt a wifi connection
    Serial.print(F("\nAttempting to connect to "));
    Serial.println(WLAN_SSID);
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
        Serial.println(F("Failed!"));
        digitalWrite( redLedPin, HIGH );
        while(1);
    }
    // If we get here we are connected
        // Try looking up the website's IP address
    Serial.print(WEBSITE); Serial.print(F(" -> "));
   while (ip == 0) {
     cc3000.getHostByName(WEBSITE, &ip);
     delay(500);
   }
    Serial.println(F("Connected!"));
    digitalWrite( greenLedPin, HIGH);
}

int consecutiveSoundCounter = 0;
int mySoundReadings[10];
int currentReadingIndex = 0;
const int soundThreshold = 40;
int upperBound = 200;

//int soundTolerances[5] = {90,80,70,60,50};
//int soundAllowances[5] = {4,4,4,4,3};
int soundTolerances[5] = {6000,55,55,50,50};
int soundAllowances[5] = {10,5,4,4,3};

 
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
void performSoundCheck()
{
     
      unsigned int sample = getSoundValue(soundPin, sampleWindow);  
      Serial.println( sample );
      if ( sample > soundTolerances[sensitivityRating-1] )   {
        mySoundReadings[currentReadingIndex] = 1;
        digitalWrite( blueLedPin, HIGH);
      } else {
        mySoundReadings[currentReadingIndex] = 0;
        digitalWrite( blueLedPin, LOW);
      }
     
      currentReadingIndex++;
      if ( currentReadingIndex == 9 ) {
        currentReadingIndex = 0;
      }
     
      int sounds = 0;
      for ( int i=0; i < 10; i++ ) {
        sounds = sounds + mySoundReadings[i];
      }
     
      if ( sounds >= soundAllowances[sensitivityRating-1] ) {
        Serial.println("Wow... OK, you are making noise!");
        for ( int i=0; i < 10; i++ ) {
           mySoundReadings[i] = 0;
        }  
        sendText();
      } else {
        delay(50);
      }

}





void sendText() {
  timeSinceLastCry = millis();
 
  Serial.println("Turn on text light and send Yo");
  digitalWrite( blueLedPin, LOW);
  digitalWrite( yellowLedPin, HIGH );

  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
      Serial.println("About to make request to Yo...");
      www.fastrprint(F("POST /yo/ HTTP/1.1\r\n"));
      www.fastrprint(F("Host: api.justyo.co\r\n"));
      //www.fastrprint(F("Accept: */*\r\n"));
      www.fastrprint(F("Content-Type: application/x-www-form-urlencoded\r\n"));
      www.fastrprint(F("Content-Length: 61\r\n\r\n"));
      www.fastrprint(F("api_token=011a7a7c-4498-43e3-9a9f-8d1629ff5153&username=IFTTT\r\n\r\n"));
      www.println();
  } else {
    Serial.println(F("Connection failed"));   
    return;
  }
 
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      //Serial.println("Reading data...");
      char c = www.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  Serial.println("Done...");
  www.close();

}

void ensureDisconnect()
{
  if ( cc3000.checkConnected() )
  {
      Serial.println(F("\n\nDisconnecting"));
      cc3000.disconnect();
  } else {
      digitalWrite( blueLedPin, LOW );
      digitalWrite( greenLedPin, LOW );
      digitalWrite( yellowLedPin, LOW );
  }
}

unsigned int getSoundValue(int soundPin, int sampleWindow) 
{
  
    unsigned int maxSample = 0;
    unsigned int minSample = 1023;
    unsigned long startMillis= millis();
    while (millis() - startMillis < sampleWindow)
    {
       unsigned int currentSample = analogRead(soundPin);
       
       if ( currentSample < 1023 ) {
       
         if ( currentSample > maxSample ) {
           maxSample = currentSample;
         }
         
         if ( currentSample < minSample ) {
           minSample = currentSample;
         }
         
       }
    }
  
    return maxSample - minSample;
} 

