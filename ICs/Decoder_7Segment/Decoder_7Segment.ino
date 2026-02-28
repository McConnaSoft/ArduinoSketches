const int decAPin=2;
const int decBPin=3;
const int decCPin=4;
const int decDPin=5;

const int buttonPin = A0;
 
void setup() {
 Serial.begin(9600);
 Serial.println("Starting...");
  pinMode(buttonPin, INPUT);
  pinMode(decAPin, OUTPUT);  //Least Significant Bit
  pinMode(decBPin, OUTPUT);
  pinMode(decCPin, OUTPUT);
  pinMode(decDPin, OUTPUT);  //Most Significant Bit
 
  to_BCD();
}
 
int count = 0; //the variable used to show the number
 
void loop() {
  if (digitalRead(buttonPin) == LOW) //if button is pressed
    {
     Serial.println("Button pressed..."); 
    count++;
    delay(200); //the delay prevent from button bouncing
    if (count == 10) //we want to count from 0 to 9!
      count = 0;
    to_BCD(); //convert to binary
    }
   
  if (count == 10)
    count = 0;
  
}
 
void to_BCD()
{
    if (count == 0) //write 0000
    {
      Serial.println("Sending 0");
      digitalWrite(decAPin, LOW);
      digitalWrite(decBPin, LOW);
      digitalWrite(decCPin, LOW);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 1) //write 0001
    {
      Serial.println("Sending 1");
      digitalWrite(decAPin, HIGH);
      digitalWrite(decBPin, LOW);
      digitalWrite(decCPin, LOW);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 2) //write 0010
    {
      Serial.println("Sending 2");
      digitalWrite(decAPin, LOW);
      digitalWrite(decBPin, HIGH);
      digitalWrite(decCPin, LOW);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 3) //write 0011
    {
      Serial.println("Sending 3");
      digitalWrite(decAPin, HIGH);
      digitalWrite(decBPin, HIGH);
      digitalWrite(decCPin, LOW);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 4) //write 0100
    {
      Serial.println("Sending 4");
      digitalWrite(decAPin, LOW);
      digitalWrite(decBPin, LOW);
      digitalWrite(decCPin, HIGH);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 5) //write 0101
    {
      Serial.println("Sending 5");
      digitalWrite(decAPin, HIGH);
      digitalWrite(decBPin, LOW);
      digitalWrite(decCPin, HIGH);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 6) //write 0110
    {
      Serial.println("Sending 6");
      digitalWrite(decAPin, LOW);
      digitalWrite(decBPin, HIGH);
      digitalWrite(decCPin, HIGH);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 7) //write 0111
    {
      Serial.println("Sending 7");
      digitalWrite(decAPin, HIGH);
      digitalWrite(decBPin, HIGH);
      digitalWrite(decCPin, HIGH);
      digitalWrite(decDPin, LOW);
    }
    
    if (count == 8) //write 1000
    {
      Serial.println("Sending 8");
      digitalWrite(decAPin, LOW);
      digitalWrite(decBPin, LOW);
      digitalWrite(decCPin, LOW);
      digitalWrite(decDPin, HIGH);
    }
    
    if (count == 9) //write 1001
    {
      Serial.println("Sending 9");
      digitalWrite(decAPin, HIGH);
      digitalWrite(decBPin, LOW);
      digitalWrite(decCPin, LOW);
      digitalWrite(decDPin, HIGH);
    }
}
