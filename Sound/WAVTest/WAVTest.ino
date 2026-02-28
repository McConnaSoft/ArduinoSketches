#include <SPI.h>
#include <SD.h>                      // need to include the SD library
//#define SD_ChipSelectPin 53  //example uses hardware SS pin 53 on Mega2560
#define SD_ChipSelectPin 10  //using digital pin 4 on arduino nano 328
#include <TMRpcm.h>           //  also need to include this library...

TMRpcm tmrpcm;   // create an object for use in this sketch
char mychar;

void setup(){

  tmrpcm.speakerPin = 9; //11 on Mega, 9 on Uno, Nano, etc

  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }
  Serial.println("Ready...");
  //tmrpcm.play("Taco.wav"); //the sound file "music" will play each time the arduino powers up, or is reset
}

void loop(){  
  
  if(Serial.available()){   
    mychar = Serial.read();

    if(mychar == 'o'){ //send the letter p over the serial monitor to start playback
      Serial.println("Playing taco");
      tmrpcm.play("Taco2.wav");
      Serial.println("Done playing taco");
    } 
    /*
    else if(mychar == 'r'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("chortle.wav");
    } else if(mychar == 'q'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("helpyou.wav");
    } else if(mychar == 'p'){
      tmrpcm.play("beware.wav");
    }
    else if(mychar == 'w'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("impresiv.wav");
    }
    else if(mychar == 't'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("seekyoda.wav");
    }
    else if(mychar == 'y'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("sensefear.wav");
    }
    else if(mychar == 'u'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("strongami.wav");
    }
    else if(mychar == 'i'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("whyhere.wav");
    }
    */
  }

}
