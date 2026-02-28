#include <Wire.h>
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>


//#define I2C_ADDR    0x3F 
#define I2C_ADDR    0x27 
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

// LCD Variables
//LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
//LiquidCrystal_I2C lcd(Rs_pin, En_pin, D4_pin, D5_pin, D6_pin, D7_pin);
LiquidCrystal_I2C lcd(0x27,16,2);
long backlightTime = 0;
const long backlightTimeout = 60000;
bool isBackLightOn = false;

bool isRelayOn = false;



enum MachineState {
  OVERRIDE, // 0
  READING // 1
};

const int TEMPERATURE_PIN = 5;
const int MOMENTARY_PIN = 2;
const int OVERRIDE_PIN = 4;
const int RELAY_PIN = 8;

long tempTime = 0;
const long tempTimeout = 5000;

MachineState currentState = MachineState::READING;

OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {

  Serial.begin(115200);
  //lcd.begin (16,2);
  lcd.init();
  sensors.begin();
  
  //lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  setLCDBackLight( true );
  lcd.home();             
  lcd.print("Smart Shart");  
  Serial.println("All set up!");

  pinMode(MOMENTARY_PIN, INPUT_PULLUP);
  pinMode(OVERRIDE_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite( RELAY_PIN, LOW );

  int overrideValue = digitalRead( OVERRIDE_PIN );
  if ( overrideValue == HIGH ) {
    currentState = MachineState::READING;
  } else {
    currentState = MachineState::OVERRIDE;
  }

  // Set default relay switch to ON if the temperature is greater than the upper bound
  float currentTemp = getTemperature();
  if ( currentTemp <= 18.5 && !isRelayOn ) {
    isRelayOn = true;
    digitalWrite( RELAY_PIN, HIGH );
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  checkBacklight();

  checkForSwitches();

  performCurrentState();
}


void checkBacklight() {
  if( millis() >= backlightTime + backlightTimeout && isBackLightOn ) {
     setLCDBackLight(false);
   }  
}

void checkForSwitches() 
{
    // Check to see if the momentary switch is pressed when the backlight is off
    if ( !digitalRead( MOMENTARY_PIN ) && !isBackLightOn ) {
      Serial.println("Button pressed, turning on back light...");
      setLCDBackLight( true );
      lcd.clear();
      lcd.home();
      lcd.print("Keep Slothy Cool");
      delay(2000);
    }

    
    int overrideSwitchValue = digitalRead( OVERRIDE_PIN );
    if ( overrideSwitchValue == HIGH  && currentState != MachineState::OVERRIDE ) {
      currentState = MachineState::OVERRIDE;
      lcd.clear();
      lcd.home();             
      lcd.print("Manual Power");
      Serial.println("Going Manual...");
      setLCDBackLight( true );
      //getTemperature();
      isRelayOn = true;
      digitalWrite( RELAY_PIN, HIGH );
    } else if ( overrideSwitchValue == LOW && currentState == MachineState::OVERRIDE ) {
      currentState = MachineState::READING;
      lcd.clear();
      lcd.home();             
      lcd.print("Robotic Power");
      Serial.println("Going Automated...");
      setLCDBackLight( true );
      //getTemperature();
      isRelayOn = true;
      digitalWrite( RELAY_PIN, HIGH );
    }
    
}

void performCurrentState() {
  switch( currentState ) {
    case MachineState::OVERRIDE:
      performOverrideState();
      break;
    case MachineState::READING:
      performReadingState();
      break;
    default:
      Serial.println("FATAL: Unsupported state.");
      break; 
  }
}

void performOverrideState() {
  // Just sit idle for now
  if ( millis() >= tempTime + tempTimeout ) {
       float currentTemp = getTemperature();
  }
  
}

void performReadingState() {
   if ( millis() >= tempTime + tempTimeout ) {
       float currentTemp = getTemperature();
       if ( currentTemp >= 18.5 && !isRelayOn ) {
          isRelayOn = true;
          digitalWrite( RELAY_PIN, HIGH );
       } else if ( currentTemp <= 17.0 && isRelayOn ) {
          isRelayOn = false;
          digitalWrite( RELAY_PIN, LOW );
       }
   } 
}

void setLCDTemperature( float temperature ) {
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(temperature,3);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
}

float getTemperature() {
  //Serial.println("Getting temperature...");
  sensors.requestTemperatures();
  float tempInCel = sensors.getTempCByIndex(0);
  setLCDTemperature(tempInCel);
  tempTime = millis();
  return tempInCel;
}

void setLCDBackLight( bool backLightOn ) {

  if ( backLightOn ) {
    //lcd.setBacklight(HIGH);
    lcd.backlight();
    backlightTime = millis();
    Serial.println("Turning light on....");
  } else {
    //lcd.setBacklight(LOW);
    lcd.noBacklight();
    Serial.println("Turning light off....");
  }

  isBackLightOn = backLightOn;

}
