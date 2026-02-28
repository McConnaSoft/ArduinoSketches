
#include <Wire.h>
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>

WiFiClient client;

const char* ssid     = "McConnellWifi";
const char* password = "Frassati1974";

//const char* host = "wifitest.adafruit.com";
const char* host = "192.168.2.202";
const int httpPort = 80;

String initialSessionID = "01234567890123456789012345678907";

#define I2C_ADDR    0x3F 
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

int n = 1;

const int BUTTON_PIN = 4;
const int TEMP_PIN = 5;
const int HEATER_PORT = 5;

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
  
long backlightTime = 0;
const long backlightTimeout = 60000;
bool isBacklightOn = false;
long tempTime = 0;
const long tempTimeout = 5000;

int portStatus[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(115200);
  
  Wire.begin(2,14);
  lcd.begin (20,4);
  sensors.begin();
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  backlightTime = millis();
  isBacklightOn = true;

  pinMode( BUTTON_PIN, INPUT_PULLUP );
  pinMode( TEMP_PIN, INPUT );
  lcd.home();                   // go home
  lcd.print("Tank Sensors:");  
  setupWifiConnection();
  makeLoginCall();
  getAllPortStatus();
  updateLCDHeaterStatus();
  getTemperature();
}

void setLCDStatusMessage( char* message ) {
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print(message);
}

void updateLCDHeaterStatus() {
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  if ( portStatus[HEATER_PORT] == 0 ) {
      lcd.print("HEATER = OFF");
  } else {
      lcd.print("HEATER = ON");
  }
}

void setLCDTemperature( float temperature ) {
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(temperature,3);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("F");
}

float getTemperature() {
  Serial.println("Getting temperature...");
  sensors.requestTemperatures();
  float tempInFar = sensors.getTempFByIndex(0);
  setLCDTemperature(tempInFar);
  tempTime = millis();
  return tempInFar;
}


void loop()
{
  
   if( millis() >= backlightTime + backlightTimeout && isBacklightOn ) {
     lcd.setBacklight(LOW);
     isBacklightOn = false;
   }

   if ( !digitalRead( BUTTON_PIN ) ) {
     delay(10);
     if ( !digitalRead( BUTTON_PIN ) ) {
       Serial.println("Button pressed");
       lcd.setBacklight(HIGH);
       backlightTime = millis();
       isBacklightOn = true;    
     } 
   }

   if ( millis() >= tempTime + tempTimeout ) {
     float currentTemp = getTemperature();
     if ( currentTemp >= 83.0 && portStatus[HEATER_PORT] == 1 ) {
       // Heater is On and temperature is too high!
       setPortStatus( HEATER_PORT, 0 );
       updateLCDHeaterStatus();
     } else if ( currentTemp < 78.0 && portStatus[HEATER_PORT] == 0 ) {
       // Temperature is good so the heater can be turned back on
       setPortStatus( HEATER_PORT, 1 );
       updateLCDHeaterStatus();
     }
      
   }
   
}

void setupWifiConnection() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  setLCDStatusMessage("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //int value = 0;
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(5000);
  //++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections

  connect();

  setLCDStatusMessage("Connected!");
  
}



void getAllPortStatus() {
  setLCDStatusMessage("Getting Status...");
  String statusUrl = "/sensors";
  String statusHTTP = String("GET ") + statusUrl + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                      "Connection: keep-alive\r\n" +
                      "Cache-Control: no-cache\r\n" +
                      "Accept: */*\r\n" +
                      "Cookie: AIROS_SESSIONID=" + initialSessionID + "\r\n\r\n";
  //Serial.println( statusHTTP );
  client.print(statusHTTP);

  while (!client.available()) {
    delay(100);
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    int outputIndex = line.indexOf("\"output\":");
    if ( outputIndex != -1 ) {
      int searchIndex = 0;
      int portIndex = 1;
      while ( line.indexOf("\"output\":", searchIndex ) != -1 ) {
        int outputPosition = line.indexOf("\"output\":", searchIndex );
        portStatus[portIndex] = (line.substring( outputPosition + 9, outputPosition + 10 )).toInt();
        Serial.print( "Port " ); Serial.print(portIndex); Serial.print(" is ");  Serial.println(portStatus[portIndex]);
        searchIndex = outputPosition + 1;
        portIndex++;
      }
    }
  }

}

void setPortStatus( int portIndex, int portValue ) {
  if ( !client.connected() ) {
    connect();
  }
  Serial.println( "*****Attempting SET*****");
  String statusUrl = "/sensors/" + String(portIndex);
  String postContent = "output=" + String(portValue);
  String setHTTP = String("POST ") + statusUrl + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: keep-alive\r\n" +
                   "Cache-Control: no-cache\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Accept: */*\r\n" +
                   "Cookie: AIROS_SESSIONID=" + initialSessionID + "\r\n" +
                   "Content-Length: " + postContent.length() + "\r\n\r\n" +
                   postContent;
  Serial.println(setHTTP);
  client.print(setHTTP);
  client.println("");
  int timeoutCheck = millis();
  while (!client.available()) {
    delay(100);
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);

    int outputIndex = line.indexOf("HTTP/1.1 200");
    if ( outputIndex != -1 ) {
      Serial.println("SUCCESS");
      portStatus[portIndex] = portValue;
    }

  }

}

void connect() {
  if (!client.connect(host, httpPort)) {
    setLCDStatusMessage("Connection failed");
    while ( true) {
    }
  }
}

void makeLoginCall() {
  
  Serial.println( "*****Attempting LOGIN*****");
  setLCDStatusMessage("Authenticating...");
  String tankLoginUrl = "/login.cgi";
  String loginContent = "username=McConnellAquarium&password=fish";
  // This will send the request to the server
  String postContent = String("POST ") + tankLoginUrl + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Cache-Control: no-cache\r\n" +
                       "Connection: keep-alive\r\n" +
                       "Accept: */*\r\n" +
                       "Content-Type: application/x-www-form-urlencoded\r\n" +
                       "Cookie: AIROS_SESSIONID=" + initialSessionID + "\r\n" +
                       "Content-Length: " + loginContent.length() + "\r\n\r\n" +
                       loginContent + "\r\n";
  Serial.println("About to make LOGIN POST: " );
  Serial.println(postContent );
  client.print(postContent);
  while (!client.available()) {
    delay(100);
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  setLCDStatusMessage("Authenticated!");
  
}







