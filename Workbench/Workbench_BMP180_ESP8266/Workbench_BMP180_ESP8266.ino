#include <SoftwareSerial.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/**** ESP8266 Stuff ****/
//SoftwareSerial esp8266(6,7);
#define esp8266 Serial

#define SSID "McConnellWifi"
#define PWD "Frassati1974"

#define HOST_IP "192.168.2.203"

#define WIFI_BAUD 9600
#define DEBUG_BAUD 115200

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];

// By default we are looking for OK\r\n
char OKrn[] = "OK\r\n";

boolean WIFI = true;

#define DEBUG

#ifdef DEBUG
    // Using SoftwareSerial for debugging requires an FTDI breakout to send the serial communications to putty or a serial monitor
    SoftwareSerial debugSerial(10,11);
    
    #define DLOG(message)     debugSerial.println(message)
    #define DLOGLN(message)   debugSerial.println(message)
    #define DBEGIN()          debugSerial.begin(DEBUG_BAUD)
#else
    #define DLOG(message)
    #define DLOGLN(message)
    #define DBEGIN()   
#endif

/**** End of ESP8266 Stuff ****/

void setup() {
  
  //Serial.begin(9600);

 // put your setup code here, to run once:
  DBEGIN();
  DLOGLN("Welcome to the Test Bed!");
  

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    DLOGLN(F("SSD1306 allocation failed"));
    while(1); // Don't proceed, loop forever
  }
  DLOGLN(F("Screen setup properly...")); 
  if(!bmp.begin())
  {
    // There was a problem detecting the BMP085 ... check your connections 
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  DLOGLN(F("BMP set properly...")); 
  
  esp8266.begin(WIFI_BAUD);    // The ESP8266 uses pins 0 & 1 for its Serial communication
  esp8266.setTimeout(5000);
  
  //hardReset();
  delay(2000);

  if ( WIFI ) {
    String response = "";
    
    if ( sendATCommandWithCheck( "AT+RST", 5000, "Ready", false ) )
    {
       DLOGLN( "ESP8266 is ready" );
    } else {
       DLOGLN( "ESP8266 module is not responding" );
       // Todo: Consider adding an LED failure indicator here
       //while(1);   //Note: Not being able to reset ins't terrible here.  Might want to log the actual response to see if we are chewcking for the right reponse.
    }
    
    
    //NOte:  It seems as though the wifi connection is preventing the OLED screen from initializing.  WE need to look into this deeper.
    // OK OK.... Why the conflict????
    
    DLOGLN("Now trying to connect");
    delay(1000); // See if this is really needed
    
    // Connect to the specified WiFi Router with 5 retries
    boolean connected = false;
    for(int i=0; i<5; i++) 
    {
      if ( connectWiFi() ) 
      {
          connected = true;
          break;
      }  
    }
    if ( !connected ) 
    {
        DLOGLN( "ESP8266 module cannot connect to your wireless network" );
        // Todo: Consider adding an LED failure indicator here
        while(1);
    }
    /*
    
    if ( sendATCommandWithCheck( "AT+CIPMUX=0", 1000, "OK", false ) )
    {
       DLOGLN( "ESP8266 is in single connection mode" );
    } else {
       DLOGLN( "ESP8266 module cannot set connection mode" );
       // Todo: Consider adding an LED failure indicator here
       while(1);   
    } 
    */
  } 
  delay(2000);

  // So this won't start if WiFi is on..... hmmmm  Is it an I2C conflict?
  

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 second

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));
  display.println(F(""));
  display.println(F("Timmer!!!!!"));
  display.display();
  
}

void loop() {
  float temperature;
  bmp.getTemperature(&temperature);
  writeTemperature( temperature );

  char temp[10];
  dtostrf(temperature,1,2,temp);
  String tempString = String(temp);
  String postDataString = "temp=" + tempString;
  char postData[64];
  strncpy(postData, postDataString.c_str(), sizeof(postData));
  postData[sizeof(postData)-1] = 0;
  
  DLOGLN( postData );
  if ( WIFI ) {
    sendHttpRequest("192.168.2.203", "8081", "POST", "/temperature", postData);
  }
  delay(1000);
}

void writeTemperature( float temp) {
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0,0);  

    display.print("Temperature: ");
    display.print(temp);
    display.println(" C");
    display.display();
}

/* ESP8266 Methods */

boolean sendHttpRequest(char hostname[], char port[], char method[], char url[], char postData[])
{
   esp8266.print("AT+CIPSTART=\"TCP\",\"");
   esp8266.print(hostname);
   esp8266.print("\",");
   String response = sendATCommand( port, 500 ); 
   if ( response.indexOf( "OK" ) != -1 || response.indexOf( "ALREAY CONNECT" ) != -1 ) 
   {
     DLOGLN( "HTTP Connection good");
   } else {
     DLOGLN( "HTTP Connection bad");
   }
   
   int messageLength = 78;   // Includes the length of the static data commented out below
   messageLength += strlen(method) + 1; // "HTTP_METHOD "     
   messageLength += strlen(url) + 1; // "URL "
   //messageLength += 10; // "HTTP/1.1\r\n"
   //messageLength += 6;  // "Host: "
   messageLength += strlen(hostname); //"HOST_NAME"
   //messageLength += 26; //"\r\nConnection: keep-alive\r\n"
   //messageLength += 32; //"User-Agent: ESP8266_IoT_Client\r\n"
   if ( postData != NULL ) {
     messageLength += 16; // "Content-Length: "
     int dataLength = strlen(postData);
     messageLength++;
     if ( dataLength > 9 ) {
       messageLength++;
     }
     if ( dataLength > 99 ) {
       messageLength++;
     }
     messageLength += 4; //"\r\n\r\n"
     messageLength += strlen(postData); //"POSTED_DATA"
     messageLength += 2; //"\r\n" 
   }
   //messageLength += 4; //"\r\n\r\n"
   DLOGLN("Got here");
   
   //String messageLengthStr = String( messageLength );
   char sendCommand[64];
   sprintf(sendCommand, "AT+CIPSEND=%d", messageLength);   

   //DLOGLN(sendCommand);
   
   
   if ( sendATCommandWithLoggingCheck( sendCommand, 500, ">" ) ) // timeout was 500
   {
     DLOGLN( "HTTP Send command good");
   } else {
     DLOGLN( "HTTP Send command bad");
   } 
   
   
   esp8266.print( method );
   esp8266.print(  " " );
   esp8266.print( url ); 
   esp8266.print( " HTTP/1.1\r\n");
   esp8266.print( "Host: ");
   esp8266.print( hostname );
   esp8266.print( "\r\nConnection: close\r\n");
   esp8266.print( "User-Agent: ESP8266_IoT_Client\r\n");
   if ( postData != NULL ) {
     esp8266.print( "Content-Length: ");
     esp8266.print( strlen(postData) );
     esp8266.print( "\r\n\r\n");
     esp8266.print( postData );
     esp8266.print( "\r\n");  
   }
   DLOGLN( "Sending....." );
   
   String closeCommand = "AT+CIPCLOSE=0";
   DLOGLN( "Sending....." );
   DLOGLN(closeCommand);
   if ( sendATCommandWithCheck( closeCommand, 500, "SEND OK", false ) ) {
     DLOGLN( "HTTP Send good");
   } else {
     DLOGLN( "HTTP Send bad");
   }

}



boolean connectWiFi()
{
   String response = sendATCommand( "AT+CWMODE=1", 2000 );
   if ( response.indexOf( "OK" ) != -1 || response.indexOf( "no change" ) != -1 ) 
   {
     DLOGLN("Mode setting successful!");
   } else {
     DLOGLN("Mode setting failed!");
     return false;
   }
   
   String cmd="AT+CWJAP=\"";
   cmd+=SSID;
   cmd+="\",\"";
   cmd+=PWD;
   cmd+="\"\r\n ";
   DLOGLN(cmd);
   if ( sendATCommandWithCheck( cmd, 3000, "OK", false ) )
   {
     DLOGLN("Connection successful!");
     return true;
   }
   
   DLOGLN("Connection failed!");
   return false;
}


String sendATCommand( String command, int timeout ) 
{
 String response = "";
 esp8266.println(command);
 long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    DLOGLN(response);
    return response;
}



boolean sendATCommandWithCheck( String command, int timeout, String check, boolean logResponse ) 
{
   if ( logResponse ) {
    
     String response = "";
     esp8266.println(command);
     long int time = millis();
        
     while( (time+timeout) > millis()) {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    DLOGLN(response);
    if ( response.indexOf( check ) >= 0 ) {
      return true;
    } 
    return false;
    
   } else {
     esp8266.println(command);
     delay(timeout);
     char *test = strdup(check.c_str());
     boolean result = esp8266.find( test );
     free(test);
     return result;
   } 
}

boolean sendATCommandWithLoggingCheck( String command, int timeout, String check ) 
{
    String response = "";
 esp8266.println(command);
 long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    DLOGLN(response);
    if ( response.indexOf( check ) >= 0 ) {
      return true;
    } 
    return false;
}

  
