
#include "LiquidCrystal.h"
#include <Adafruit_CC3000.h>
#include <LiquidCrystal_I2C.h>
//#include <ccspi.h>
//#include <SPI.h>
//#include <string.h>
//#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  11 //5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed
                                  
LiquidCrystal_I2C lcd2(0x3f, 16, 2); 

#define WLAN_SSID       "McConnellWifi"           
#define WLAN_PASS       "Frassati1974"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.


#define IFTTT_WEBSITE           "maker.ifttt.com"
#define IFTTT_KEY               "c0R5GKmeiPTYle2oBxxlod" // >>> Change IFTTT Tim
#define IFTTT_EMAIL_EVENT_NAME  "Send_Email_From_Maker_Post"
uint32_t ifttt_ip = 0;

#define postButtonPin 36

char welcomeString[] = "    Welcome";

//const char server_ip_string[] = "192.168.2.48";
uint32_t serverIP = cc3000.IP2U32(192, 168, 2, 48);

// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(8,9,4,5,6,7);

boolean WIFI_CONNECTED = false;
boolean NO_WIFI = false;

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  pinMode( postButtonPin, INPUT );
  
  lcd2.init();
  lcd2.backlight();

  if ( NO_WIFI ) {
    WIFI_CONNECTED = false; 
  } else {
    if (!cc3000.begin())
    {
      Serial.println(F("Couldn't begin()! Check your wiring?"));
      WIFI_CONNECTED = false;
    } else {
      WIFI_CONNECTED = true;
    }
  }

  if ( WIFI_CONNECTED ) {
    updateLCDDisplay( F("    Welcome"), F(" Connecting...") );
    
    Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
      Serial.println(F("WiFi failed to connect!"));
      updateLCDDisplay( F("    Welcome"), F("Wifi Failed") );
      //while (1);
      WIFI_CONNECTED = false;
    }
    
  }
  
  if ( WIFI_CONNECTED ) {
    Serial.println(F("Connected!"));
    while (ifttt_ip == 0) {
      cc3000.getHostByName(IFTTT_WEBSITE, &ifttt_ip);
      delay(500);
    }
    updateLCDDisplay( F("    Welcome"), F("Wifi Connected") );
  } else {
    updateLCDDisplay( F("    Welcome"), F("No Wifi Mode  ") );
  }

}

void loop()
{
   if ( digitalRead( postButtonPin ) == LOW ) {
      Serial.println("Detected button push, posting to Node server...");
      updateLCDDisplay( "       DB       ", "     Med Taken " );
      sendMedicationTakenPost();
      updateLCDDisplay( "       DB       ", "     Not Taken " );
      sendMedicationNotTakenPost();
      updateLCDDisplay( "      IFTTT     ", "    Send Email " );
      sendEmailIFTTTPost(); 
   }
}


void sendMedicationTakenPost() {
  sendPost( "patient_name=Anooj%20Shah&medication_event=Medication%20Taken" );
}

void sendMedicationNotTakenPost() {
  sendPost( "patient_name=Anooj%20Shah&medication_event=Medication%20Not%20Taken" );
}

void sendPost( String postDataString ) {

  if ( !WIFI_CONNECTED ) {
    delay(2000);
    return;
  } 
  
  char postData[256];
  strncpy(postData, postDataString.c_str(), sizeof(postData));
  postData[sizeof(postData)-1] = 0;



  Adafruit_CC3000_Client client = cc3000.connectTCP(serverIP, 3000);

  
  if (client.connected()) {
      
      Serial.println("About to send data to Node...");
      
      client.fastrprintln(F("POST /addMedicineInfo HTTP/1.1"));
      Serial.print("POST /addMedicineInfo HTTP/1.1\r\n");
      client.fastrprintln(F("Host: 192.168.2.48:3000"));
      Serial.print("Host: 192.168.2.48:3000\r\n");
      client.fastrprint(F("Content-Length: "));
      Serial.print("Content-Length: " );
      client.print(postDataString.length());
      Serial.print(postDataString.length());
      client.fastrprint(F("\r\n"));
      Serial.print("\r\n");
      client.fastrprint(F("Content-Type: application/x-www-form-urlencoded\r\n\r\n"));
      Serial.print("Content-Type: application/x-www-form-urlencoded\r\n\r\n");
      client.fastrprint(postData);
      Serial.println(postData);
      client.println();
      
  } else {
    Serial.println(F("Connection failed"));   
    return;
  }
  bool response = false;
  unsigned long lastRead = millis();
  while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (client.available()) {
      //Serial.println("Reading data...");
      char c = client.read();
      Serial.print(c);
      response = true;
      lastRead = millis();
    }
  }
  if ( response ) {
    Serial.println(F("Done with success..."));
  } else {
    Serial.println(F("Done with failure..."));
    //client.close();
  }
  freeRam();
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

  lcd2.setCursor(0,0);
  //text to print
  lcd2.print(firstLine);
  // set cusor position to start of next line
  lcd2.setCursor(0,1);
  lcd2.print(secondLine);
  
  freeRam();

}

int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  int freemem = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  Serial.print( F("Memory free: ") );
  Serial.println( freemem );
}

void sendEmailIFTTTPost() {
  sendIFTTTPost( "{}", IFTTT_EMAIL_EVENT_NAME );
}

void sendIFTTTPost( String postDataString, String eventName ) {

  if ( !WIFI_CONNECTED ) {
    delay(2000);
    return;
  } 
  
  char postData[256];
  strncpy(postData, postDataString.c_str(), sizeof(postData));
  postData[sizeof(postData) - 1] = 0;

  Adafruit_CC3000_Client iftttClient = cc3000.connectTCP(ifttt_ip, 80);

  if (iftttClient.connected()) {

    Serial.println(F("About to send data to IFTTT ..."));
    

    iftttClient.fastrprint(F("POST /trigger/"));
    iftttClient.print( eventName );
    iftttClient.fastrprint(F("/with/key/"));
    iftttClient.print( IFTTT_KEY );
    iftttClient.fastrprintln(F(" HTTP/1.1"));

    Serial.print(F("POST /trigger/Send_Email_From_Maker_Post/with/key/"));
    Serial.print( F(IFTTT_KEY) );
    Serial.print( F(" HTTP/1.1\r\n") );
    iftttClient.fastrprintln(F("Host: maker.ifttt.com"));
    Serial.print(F("Host: maker.ifttt.com\r\n"));
    iftttClient.fastrprint(F("Content-Length: "));
    Serial.print(F("Content-Length: "));
    iftttClient.print(postDataString.length());
    Serial.print(postDataString.length());
    iftttClient.fastrprint(F("\r\n"));
    Serial.print(F("\r\n"));
    iftttClient.fastrprint(F("Content-Type: application/json\r\n\r\n"));
    Serial.print(F("Content-Type: application/json\r\n\r\n"));
    iftttClient.fastrprint(postData);
    Serial.println(postData);
    iftttClient.println();

  } else {
    Serial.println(F("IFTTT Connection failed"));
    return;
  }
  unsigned long lastRead = millis();
  bool successfulResponse = false;
  Serial.println(F("Waiting for an IFTTT response"));
  while (iftttClient.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (iftttClient.available()) {
      //Serial.println("Reading data...");
      char c = iftttClient.read();
      Serial.print(c);
      lastRead = millis();
      successfulResponse = true;
    }
  }
  if ( successfulResponse ) {
    Serial.println(F("Done with succcess..."));
  } else {
    Serial.println(F("Done with failure..."));
  }
  freeRam();
  iftttClient.close();
  delay(200);

}


