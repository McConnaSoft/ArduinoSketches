/*
 * A simple sketch that uses WiServer to get the hourly weather data from LAX and prints
 * it via the Serial API
 */
#define __PROG_TYPES_COMPAT__
#include <WiServer.h>

#define WIRELESS_MODE_INFRA  1
#define WIRELESS_MODE_ADHOC 2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,2,101}; // IP address of WiShield
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
// End of wireless configuration parameters ----------------------------------------


// Function that prints data from the server
void printData(char* data, int len) {
  Serial.println("We got a response...");
  Serial.print("The length is..." );
  Serial.println( len );
  // Print the data returned by the server
  // Note that the data is not null-terminated, may be broken up into smaller packets, and 
  // includes the HTTP header. 
  while (len-- > 0) {
    Serial.print(*(data++));
  } 
}


// IP Address for www.weather.gov  
//uint8 ip[] = {184,84,243,215};
//uint8 ip[] = {173,194,123,114};
uint8 ip[] = {104,93,172,206};

// A request that gets the latest METAR weather data for LAX
GETrequest getWeather(ip, 80, "api.wunderground.com", "/api/58724a06f6640a15/forecast/q/Canada/Kitchener.json");
//GETrequest getWeather(ip, 80, "www.google.com", "/");

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
long updateTime = 0;

void loop(){

  // Check if it's time to get an update
  if (millis() >= updateTime) {
    Serial.println("Going to submit a weather request");
    getWeather.submit();    
    // Get another update one hour from now
    updateTime += 1000 * 60 * 5;
  }
  
  // Run WiServer
  WiServer.server_task();
 
  delay(10);
}
