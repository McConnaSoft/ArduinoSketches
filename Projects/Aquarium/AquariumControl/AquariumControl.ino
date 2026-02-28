#include <ESP8266WiFi.h>

const char* ssid     = "McConnellWifi";
const char* password = "Frassati1974";

const int CHEATO_LIGHT = 1;
const int CHEATO_LIGHT_PIN = 5; // GREEN BUTTON

const int POWER_HEAD = 3;
const int MAIN_PUMP = 4;
const int PUMPS_PIN = 4; // YELLOW BUTTON


const int REFRESH = 7;
const int REFRESH_PIN = 14; // GRAY BUTTON
const int WHITE_LIGHT = 6;
const int WHITE_LIGHT_PIN = 13; // RED BUTTON
const int ACTINIC_LIGHT = 8;
const int ACTINIC_LIGHT_PIN = 12; // BLUE BUTTON

const int RED_LED_PIN = 16;
const int GREEN_LED_PIN = 15;

const int BUTTON_TIMEOUT = 7000;

int portStatus[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

enum LoginCallback { CALLBACK_NONE, CALLBACK_GET, CALLBACK_SET };

WiFiClient client;

//const char* host = "wifitest.adafruit.com";
const char* host = "192.168.2.202";
const int httpPort = 80;

String initialSessionID = "01234567890123456789012345678907";
void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode( CHEATO_LIGHT_PIN, INPUT_PULLUP );
  pinMode( PUMPS_PIN, INPUT_PULLUP );
  pinMode( REFRESH_PIN, INPUT_PULLUP );
  pinMode( WHITE_LIGHT_PIN, INPUT_PULLUP );
  pinMode( ACTINIC_LIGHT_PIN, INPUT_PULLUP );
  pinMode( RED_LED_PIN, OUTPUT );
  pinMode( GREEN_LED_PIN,  OUTPUT );

  digitalWrite( RED_LED_PIN, LOW );
  digitalWrite( GREEN_LED_PIN, LOW );

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  digitalWrite( GREEN_LED_PIN, LOW );
  digitalWrite( RED_LED_PIN, HIGH );
  makeLoginCall( CALLBACK_NONE, 0, 0 );
  digitalWrite( GREEN_LED_PIN, HIGH );
  digitalWrite( RED_LED_PIN, LOW );
  getAllPortStatus(true);
}

boolean busyFlip = false;

void loop() {

  testPin( CHEATO_LIGHT_PIN, CHEATO_LIGHT );
  testPin( PUMPS_PIN, 0 );
  testPin( REFRESH_PIN, REFRESH );
  testPin( WHITE_LIGHT_PIN, WHITE_LIGHT );
  //testPin( ACTINIC_LIGHT_PIN, ACTINIC_LIGHT );

}

void connect() {
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    digitalWrite( RED_LED_PIN, HIGH );
    while ( true) {
    }
  }
}

void closeConnection() {
  client.stop();
}

void testPin( int pin, int portIndex ) {
  
  
  if ( !digitalRead( pin ) ) {
    delay(10);
    if ( !digitalRead( pin ) ) {
      if ( pin == PUMPS_PIN ) {
        flipPumps();
      //} else if ( pin == ACTINIC_LIGHT_PIN ) {
      //  makeLoginCall();
      //  getAllPortStatus();
      } if ( pin == REFRESH_PIN ) {
        digitalWrite( RED_LED_PIN, HIGH ); 
        getAllPortStatus(true);
        digitalWrite( RED_LED_PIN, LOW ); 
        
      } else {
        flipPort( portIndex );
      }
    }
  }
  

}

void flipPort( int portIndex ) {
  digitalWrite( RED_LED_PIN, HIGH ); 
  //getAllPortStatus( true );
  int status = portStatus[portIndex];
  setPortStatus( true, portIndex, (status == 0) ? 1 : 0 );
  digitalWrite( RED_LED_PIN, LOW ); 
}

void flipPumps() {
  digitalWrite( RED_LED_PIN, HIGH ); 
  //getAllPortStatus( true );
  int status = portStatus[3];
  setPortStatus( true, POWER_HEAD, (status == 0) ? 1 : 0 );
  setPortStatus( true, MAIN_PUMP, (status == 0) ? 1 : 0 );
  digitalWrite( RED_LED_PIN, LOW ); 
}

void flipGreenLight() {
  busyFlip = !busyFlip;
  if ( busyFlip ) {
    digitalWrite( GREEN_LED_PIN, HIGH );
  } else {
    digitalWrite( GREEN_LED_PIN, LOW );
  }
}

void getAllPortStatus(boolean retry) {
  if ( !client.connected() ) {
    connect();
  }
  String statusUrl = "/sensors";
  String statusHTTP = String("GET ") + statusUrl + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                      "Connection: keep-alive\r\n" +
                      "Cache-Control: no-cache\r\n" +
                      "Accept: */*\r\n" +
                      "Cookie: AIROS_SESSIONID=" + initialSessionID + "\r\n\r\n";
  Serial.println( statusHTTP );
  client.print(statusHTTP);
  client.println("");
  
  while (!client.available()) {
    Serial.print("x");
    flipGreenLight();
    delay(2000);
  }
  boolean success = false;
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    int outputIndex = line.indexOf("\"output\":");
    if ( outputIndex != -1 ) {
      int searchIndex = 0;
      int portIndex = 1;
      while ( line.indexOf("\"output\":", searchIndex ) != -1 ) {
        success = true;
        int outputPosition = line.indexOf("\"output\":", searchIndex );
        portStatus[portIndex] = (line.substring( outputPosition + 9, outputPosition + 10 )).toInt();
        Serial.print( "Port " ); Serial.print(portIndex); Serial.print(" is ");  Serial.println(portStatus[portIndex]);
        searchIndex = outputPosition + 1;
        portIndex++;
      }
      digitalWrite( GREEN_LED_PIN, HIGH );
    }
  }

  if ( !success && retry ) {
    Serial.println("Not logged in....");
    makeLoginCall( CALLBACK_GET, 0, 0 );
  }

}

void setPortStatus( boolean retry, int portIndex, int portValue ) {
  digitalWrite( RED_LED_PIN, LOW );
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
    //if ( millis() - timeoutCheck > BUTTON_TIMEOUT ) {
    //  digitalWrite( RED_LED_PIN, HIGH );
    //  closeConnection();
    //  return;
    //}
    Serial.print("y");
    flipGreenLight();
    delay(100);
  }
  boolean success = false;
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);

    int outputIndex = line.indexOf("HTTP/1.1 200");
    if ( outputIndex != -1 ) {
      Serial.println("SUCCESS");
      success = true;
      portStatus[portIndex] = portValue;
    }

  }
  if ( !success && retry ) {
     makeLoginCall( CALLBACK_SET, portIndex, portValue );
  }
  digitalWrite( GREEN_LED_PIN, HIGH );

}

void makeLoginCall( LoginCallback callback, int param1, int param2 ) {
  Serial.println( "*****Attempting LOGIN*****");
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
    Serial.print(".");
    flipGreenLight();
    delay(100);
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  //digitalWrite( GREEN_LED_PIN, HIGH );

  switch ( callback ) {
    case CALLBACK_GET:
      getAllPortStatus(false);
      break;
    case CALLBACK_SET:
      setPortStatus(false, param1, param2);
      break;
    case CALLBACK_NONE:
      break;  
    default:  
      // Nothing
      Serial.println("Unknown callback...");
  }

  
  
}
