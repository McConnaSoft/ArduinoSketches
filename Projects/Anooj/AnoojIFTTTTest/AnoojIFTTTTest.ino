
#include "LiquidCrystal.h"
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  11 //5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID      "<YOUR_HOME_WIFI_NETWORK_NAME>"           
#define WLAN_PASS      "<YOUR_HOME_WIFI_PASSWORD>"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

#define postButtonPin 36

#define IFTTT_WEBSITE           "maker.ifttt.com"
#define IFTTT_KEY               "<YOUR_IFTTT_DEV_KEY_HERE>"
#define IFTTT_EMAIL_EVENT_NAME  "Send_Email_From_Maker_Post"
#define IFTTT_SMS_EVENT_NAME    "Send_SMS_From_Maker_Post"
uint32_t ifttt_ip;



const char server_ip_string[] = "192.168.2.48";
uint32_t serverIP = cc3000.IP2U32(192, 168, 2, 48);

// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(8,9,4,5,6,7);

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  pinMode( postButtonPin, INPUT );
  
  updateLCDDisplay( "    Welcome", " Connecting..." );
  /* Initialise the CC3000 module */
  Serial.println(F("\nInitializing Wifi Breakout..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }

  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("WiFi failed to connect!"));
    updateLCDDisplay( "    Welcome", "Wifi Failed" );
    while(1);
  }
   
  Serial.println(F("Connected!"));

  // Get IFTTT IP Address
  while (ifttt_ip == 0) {
    cc3000.getHostByName(IFTTT_WEBSITE, &ifttt_ip);
    delay(500);
  }

  
  updateLCDDisplay( "    Welcome", "Wifi Connected" );

}

void loop()
{
   if ( digitalRead( postButtonPin ) == LOW ) {
      Serial.println("Detected button push, posting to Node server...");
      sendEmailIFTTTPost();
   }
}


void sendEmailIFTTTPost() {
  sendIFTTTPost( "{}", IFTTT_EMAIL_EVENT_NAME );
}

void sendSMSIFTTTPost() {
  sendIFTTTPost( "{}", IFTTT_SMS_EVENT_NAME );
}


void sendIFTTTPost( String postDataString, String eventName ) {
  
  Adafruit_CC3000_Client client = cc3000.connectTCP(ifttt_ip, 80);

  char postData[256];
  strncpy(postData, postDataString.c_str(), sizeof(postData));
  postData[sizeof(postData)-1] = 0;

  
  if (client.connected()) {
      
      Serial.println("About to send data to Node...");
      
      client.fastrprint(F("POST /trigger/"));
      client.print( eventName );
      client.fastrprint(F("/with/key/"));
      client.print( IFTTT_KEY );
      client.fastrprintln(F(" HTTP/1.1"));
      Serial.print("POST /trigger/Send_Email_From_Maker_Post/with/key/");
      Serial.print( IFTTT_KEY ); 
      Serial.print( " HTTP/1.1\r\n" );
      client.fastrprintln(F("Host: maker.ifttt.com"));
      Serial.print("Host: maker.ifttt.com\r\n");
      client.fastrprint(F("Content-Length: "));
      Serial.print("Content-Length: " );
      client.print(postDataString.length());
      Serial.print(postDataString.length());
      client.fastrprint(F("\r\n"));
      Serial.print("\r\n");
      client.fastrprint(F("Content-Type: application/json\r\n\r\n"));
      Serial.print("Content-Type: application/json\r\n\r\n");
      client.fastrprint(postData);
      Serial.println(postData);
      client.println();
      
  } else {
    Serial.println(F("Connection failed"));   
    return;
  }
  unsigned long lastRead = millis();
  while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (client.available()) {
      //Serial.println("Reading data...");
      char c = client.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  Serial.println("Done...");
  client.close();
  delay(200);
  
}

void updateLCDDisplay( String firstLine, String secondLine ) {
  // set cursor position to start of first line on the LCD
  lcd.setCursor(0,0);
  //text to print
  lcd.print(firstLine);
  // set cusor position to start of next line
  lcd.setCursor(0,1);
  lcd.print(secondLine);

}




