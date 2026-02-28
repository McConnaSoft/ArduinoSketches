#define SOUNDLEDPIN 3
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode( SOUNDLEDPIN, OUTPUT);
}

void loop() {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1000;
  unsigned int sample;
   // collect data for 50 mS
   while (millis() - startMillis < 50)
   {
      sample = analogRead(A0);
      if (sample < 900)  // toss out spurious readings
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
  if ( peakToPeak  < 70 ) {
    peakToPeak = 70;
  } 
  Serial.println(peakToPeak);
  analogWrite( SOUNDLEDPIN, map(peakToPeak,70, 900 ,0,255) );
  
}
