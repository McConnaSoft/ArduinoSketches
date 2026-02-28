
#define LED_PIN     5


void setup() {
  Serial.begin(9600);


  pinMode(LED_PIN, OUTPUT);

}

void loop() {
  performButton3();
 
}



void performButton3()
{

  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int sample;
   // collect data for 50 mS
   while (millis() - startMillis < 50)
   {
      sample = analogRead(A0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  //if ( peakToPeak  < 70 ) {
  //  peakToPeak = 70;
  //} 
  Serial.println(peakToPeak);
  analogWrite( LED_PIN, map(peakToPeak,0,1023,0,255) );
}



