/*
 * A simple sketch that uses WiServer to get the hourly weather data from LAX and prints
 * it via the Serial API
 */
#define __PROG_TYPES_COMPAT__
#include <WiServer.h>
#include <ArduinoJson.h>

#define WIRELESS_MODE_INFRA  1
#define WIRELESS_MODE_ADHOC 2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,2,241}; // IP address of WiShield
unsigned char gateway_ip[] = {192,168,2,1}; // router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};  // subnet mask for the local network
const prog_char ssid[] PROGMEM = {"McConnellWifi"};   // max 32 bytes

unsigned char security_type = 3;  // 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"Frassati1974"}; // max 64 characters

// WEP 128-bit keys
// sample HEX keys

const prog_uchar wep_keys[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, // Key 0
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Key 1
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Key 2
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Key 3
        };


// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;



String getResponse;
// End of wireless configuration parameters ----------------------------------------


// Function that prints data from the server
void printData(char* data, int len) {
  
  if ( len == 0 ) {
    handleResponse();
    return;
  }
  
  data[len] = '\0';
  getResponse += data; 
  
}



// IP Address for www.weather.gov  
uint8 ip[] = {98,137,200,255};

// Yahoo Weather API:  https://query.yahooapis.com/v1/public/yql?q=select%20item.forecast%20from%20weather.forecast%20where%20woeid%20%3D%204051%20and%20u%3D'c'&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys
GETrequest getWeather(ip, 80, "query.yahooapis.com", "/v1/public/yql?q=select%20item.forecast%20from%20weather.forecast%20where%20woeid%20%3D%204051%20and%20u%3D'c'&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys");

void setup() {
  Serial.begin(57600);
  Serial.println("Getting ready to roll");
    
  // Initialize WiServer (we'll pass NULL for the page serving function since we don't need to serve web pages) 
  WiServer.init(NULL);
  Serial.println("Done init");
  // Enable Serial output and ask WiServer to generate log messages (optional)
  
  WiServer.enableVerboseMode(true);

  // Have the processData function called when data is returned by the server
  getWeather.setReturnFunc(printData);
}


// Time (in millis) when the data should be retrieved 
unsigned long timeSinceLastUpdate = 0;

void loop(){
  
   unsigned long currentTime = millis();

  // Check if it's time to get an update
  if ( timeSinceLastUpdate == 0 || abs(currentTime - timeSinceLastUpdate) > 300000 ) {
    Serial.println("Going to submit a weather request");
    //getResponse.clear();
    getResponse = "";
    getResponse += '/0';
    getWeather.submit();    
    timeSinceLastUpdate = millis();
    Serial.println("Submission request done");
  }
  
  // Run WiServer
  WiServer.server_task();
  delay(10);
}


void handleResponse() {
  int firstOpeningJSONBracket = getResponse.indexOf('{');
  int lastClosingJSONBracket = getResponse.lastIndexOf('}');
  Serial.println( "MESSAGING" );
  getResponse = getResponse.substring( firstOpeningJSONBracket, lastClosingJSONBracket + 1) + '\0';
  //Serial.println( getResponse.substring( firstOpeningJSONBracket, lastClosingJSONBracket + 1) );
  Serial.println( getResponse );
  
  //char json[] = "{\"query\":{\"count\":5,\"created\":\"2015-11-08T03:57:59Z\",\"lang\":\"en-US\",\"results\":{\"channel\":[{\"item\":{\"forecast\":{\"code\":\"29\",\"date\":\"7 Nov 2015\",\"day\":\"Sat\",\"high\":\"11\",\"low\":\"1\",\"text\":\"Partly Cloudy\"}}},{\"item\":{\"forecast\":{\"code\":\"32\",\"date\":\"8 Nov 2015\",\"day\":\"Sun\",\"high\":\"9\",\"low\":\"-2\",\"text\":\"Sunny\"}}},{\"item\":{\"forecast\":{\"code\":\"32\",\"date\":\"9 Nov 2015\",\"day\":\"Mon\",\"high\":\"12\",\"low\":\"1\",\"text\":\"Sunny\"}}},{\"item\":{\"forecast\":{\"code\":\"11\",\"date\":\"10 Nov 2015\",\"day\":\"Tue\",\"high\":\"11\",\"low\":\"3\",\"text\":\"Showers\"}}},{\"item\":{\"forecast\":{\"code\":\"30\",\"date\":\"11 Nov 2015\",\"day\":\"Wed\",\"high\":\"14\",\"low\":\"7\",\"text\":\"Partly Cloudy\"}}}]}}}";
  
  //JsonObject& root = jsonBuffer.parseObject( getResponse.substring( firstOpeningJSONBracket, lastClosingJSONBracket + 1) );
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject( const_cast<char*>(getResponse.c_str()) );
  //JsonObject& root = jsonBuffer.parseObject( json );

  if ( !root.success() ) {
    Serial.println( "FATAL ERROR: JSON Parsing Issue!" );
    return;
  } 
  /*
  JsonObject& root2 = jsonBuffer.parseObject( const_cast<char*>(getResponse.c_str()) );
  if ( !root2.success() ) {
    Serial.println( "ROOT 2 FATAL ERROR: JSON Parsing Issue!" );
    return;
  } else {
    Serial.println( "Able to do it back to back" );
  }
  */
  
  const char* firstHigh = root["query"]["results"]["channel"][0]["item"]["forecast"]["high"];
  Serial.print( "The first high is: " );
  Serial.println( firstHigh );
  const char* secondHigh = root["query"]["results"]["channel"][1]["item"]["forecast"]["high"];
  Serial.print( "The second high is: " );
  Serial.println( secondHigh );
  const char* lastText = root["query"]["results"]["channel"][4]["item"]["forecast"]["text"];
  Serial.print( "Last text is: " );
  Serial.println( lastText );
  
  Serial.println( "DONE RESPONSE" );
   
}

