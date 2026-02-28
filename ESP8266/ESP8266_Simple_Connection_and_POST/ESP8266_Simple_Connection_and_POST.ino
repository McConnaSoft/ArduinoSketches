#include <SoftwareSerial.h>

#define esp8266 Serial

#define SSID "McConnellWifi"
#define PWD "Frassati1974"

#define HOST_IP "192.168.2.203"

#define WIFI_BAUD 115200
#define DEBUG_BAUD 115200
//#define ESP8266_RST 4 
//#define ESP8266_HARD_RESET_DURATION 1500

//#define tempSensorPin A0

unsigned long currentTimestamp = 0;
unsigned long httpTimestamp = 0;
unsigned long ramTimestamp;


#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];

// By default we are looking for OK\r\n
char OKrn[] = "OK\r\n";

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


void setup() 
{
  // put your setup code here, to run once:
  DBEGIN();
  DLOGLN("Welcome to the Test Bed!");
  esp8266.begin(WIFI_BAUD);    // The ESP8266 uses pins 0 & 1 for its Serial communication
  esp8266.setTimeout(5000);
  //hardReset();
  delay(2000);
  
  String response = "";
  if ( sendATCommandWithCheck( "AT+RST", 3000, "Ready", false ) )
  {
     DLOGLN( "ESP8266 is ready" );
  } else {
     DLOGLN( "ESP8266 module is not responding" );
     // Todo: Consider adding an LED failure indicator here
     //while(1);   
  }
  
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
  if ( sendATCommandWithCheck( "AT+CIPMUX=0", 1000, "OK", false ) )
  {
     DLOGLN( "ESP8266 is in single connection mode" );
  } else {
     DLOGLN( "ESP8266 module cannot set connection mode" );
     // Todo: Consider adding an LED failure indicator here
     while(1);   
  }
  
}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  //int rawvoltage= analogRead(tempSensorPin);
  //float millivolts= (rawvoltage/1024.0) * 5000;
  float celsius= 74.5;
  httpTimestamp = currentTimestamp;
       
        
  char temp[10];
  dtostrf(celsius,1,2,temp);
  String tempString = String(temp);
  String postDataString = "temp=" + tempString;
  char postData[64];
  strncpy(postData, postDataString.c_str(), sizeof(postData));
  postData[sizeof(postData)-1] = 0;
  
  DLOGLN( postData );
  
  sendHttpRequest("192.168.2.203", "8081", "POST", "/temperature", postData);
  
  delay(4000);
}


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
