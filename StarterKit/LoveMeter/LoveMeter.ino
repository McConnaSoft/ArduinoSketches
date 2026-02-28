
const int sensorPin = A0;
const float baselineTemp = 23.0;

void setup() {

  Serial.begin(9600);    // Open a serial port at a speed of 9600 bits per second
                         // This is used to send text the Serial Monitor that comes with the Arduino IDE
                         // Serial.print and Serial.println are used to log information to the Serial Monitor
  
  for (int pinNumber = 2; pinNumber < 5; pinNumber++ ) {
    pinMode(pinNumber,OUTPUT);
    digitalWrite(pinNumber,LOW);
  }
  
}

void loop() {
  int sensorVal = analogRead(sensorPin);    // Always a value between 0 and 1023 and represents the voltage on the pin
  
  Serial.print("Sensor value: ");
  Serial.print(sensorVal);
  
  // Convert the ADC reading to voltage
  float voltage = (sensorVal/1024.0) * 5.0;   // The voltage will be between 0 and 5
                                              // This will translate the TMP36 sensor value to voltage
  
  Serial.print(", Volts: ");
  Serial.print(voltage);
  
  Serial.print(", degrees C: ");
  
  // Convert the voltage to temperature in degrees
  float temperature = (voltage - .5) * 100;
  Serial.println(temperature);

  if ( temperature < baselineTemp )
  {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  } else if ( temperature >= baselineTemp+2 && temperature < baselineTemp+4 )
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  } else if ( temperature >= baselineTemp+4 && temperature < baselineTemp+6 )
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
  } else if ( temperature >= baselineTemp+6 )
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
  }

  delay(1);
  
}
