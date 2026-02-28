#include <IRLib.h>

#define YELLOW_PIN 4
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
//#define RAWBUF 300
 
//Create a receiver object to listen on pin 11
IRrecv My_Receiver(11);
 
//Create a decoder object
IRdecode My_Decoder;
//unsigned int Buffer[RAWBUF];

// Storage for the recorded code
IRTYPES codeType;          // The type of code
unsigned long codeValue;   // The data bits if type is not raw
int codeBits;              // The length of the code in bits

unsigned long yellowTime = 0;
unsigned long redTime = 0;
unsigned long greenTime = 0;
unsigned long blueTime = 0;
 
void setup()
{
  Serial.begin(9600);
  
  pinMode( YELLOW_PIN, OUTPUT );
  pinMode( RED_PIN, OUTPUT );
  pinMode( GREEN_PIN, OUTPUT );
  pinMode( BLUE_PIN, OUTPUT );

  digitalWrite( YELLOW_PIN, LOW );
  digitalWrite( RED_PIN, LOW );
  digitalWrite( GREEN_PIN, LOW );
  digitalWrite( BLUE_PIN, LOW );

  My_Receiver.enableIRIn(); // Start the receiver
  //My_Decoder.UseExtnBuf(Buffer);
}
 
void loop() {
  unsigned long currentTime = millis();

  if ( currentTime > yellowTime + 1000 ) {
     digitalWrite( YELLOW_PIN, LOW );
  }

  if ( currentTime > redTime + 1000 ) {
     digitalWrite( RED_PIN, LOW );
  }

  if ( currentTime > greenTime + 1000 ) {
     digitalWrite( GREEN_PIN, LOW );
  }

  if ( currentTime > blueTime + 1000 ) {
     digitalWrite( BLUE_PIN, LOW );
  }

  
//Continuously look for results. When you have them pass them to the decoder
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();    //Decode the data
    //My_Decoder.DumpResults(); //Show the results on serial monitor
    codeType = My_Decoder.decode_type;
    if ( codeType == SONY ) {
      switch ( My_Decoder.value ) {
        case 0x11111:
          digitalWrite( YELLOW_PIN, HIGH );
          yellowTime = millis();
          break;
        case 0x22222:
          digitalWrite( RED_PIN, HIGH );
          redTime = millis();
          break;
        case 0x33333:
          digitalWrite( GREEN_PIN, HIGH );
          greenTime = millis();
          break; 
        case 0x44444:
          digitalWrite( BLUE_PIN, HIGH );
          blueTime = millis();
          break;
       default:
          break;          
      }
    }
    My_Receiver.resume();     //Restart the receiver
  }
}
