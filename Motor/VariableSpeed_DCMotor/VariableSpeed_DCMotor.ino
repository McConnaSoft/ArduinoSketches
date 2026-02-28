const int motorpin = 9;
int motor_speed = 0;

void setup()
{
  Serial.begin(57600);
  pinMode(motorpin, OUTPUT); 
  analogWrite(motorpin, 20);
}

void loop()
{
  /*
  for(motor_speed = 10; motor_speed <= 40; motor_speed += 2)
  {
    printSpeed(motor_speed);
    //printSpeed(0);
    analogWrite(motorpin, motor_speed);
    //analogWrite(motorpin, 10);
    delay(1000);
  }
  ///delay(5000);
  */
}

void printSpeed(int motor_speed)
{
  Serial.print("Current Speed: ");
  Serial.println(motor_speed);
}
