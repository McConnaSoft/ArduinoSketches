/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "ESP8266WiFi.h"
#include "WiFiClient.h"

#define SSID "McConnellWifi"
#define PWD "Frassati1974"

#define HOST_IP "192.168.2.203"

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(SSID, PWD);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Success!");
  
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Setup done");
  
}

void loop() {

  String content = "";
  char character;
  while ( Serial.available() ) {
      character = Serial.read();
      content.concat(character);
      delay(10);
  } 
  
  if ( content != "" ) {
    postData( content );
  }
 
}

void postData( String data ) {

  if ( !client.connected() ) {
    client.connect(HOST_IP, 8081);
  }

  if ( client.connected() ) {
    String PostData = data;
    Serial.println("Posting data:");
    Serial.println(PostData);
    client.println("POST /temperature HTTP/1.1");
    client.print("Host: ");
    client.println(HOST_IP);
    client.println("Cache-Control: no-cache");
    client.println( "Connection: close");
    client.println( "User-Agent: ESP8266_IoT_Client");
    client.print("Content-Length: ");
    client.println(PostData.length());  
    client.println();
    client.print(PostData);
  }

}
