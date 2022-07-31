#include <GyverTimer.h>            //TIMER LIBRARY https://alexgyver.ru/gyvertimer/
#include <Wire.h>                  //DISPLAY LIBRARY
#include <LiquidCrystal_I2C.h>     //DISPLAY LIBRARY

LiquidCrystal_I2C lcd(0x27,16,2);  
GTimer LightON (MS,3000000); //LAMP LIGHTNING TIME (50 MIN)
GTimer LightOFF (MS,600000); //DELAY FOR COOLING OF LAMP AND POWER UNIT (10 MIN)
GTimer hours24 (MS,86400000); //FULL DAY CYCLE (24 HR)
GTimer HumidityTime (MS,900000); //TIME BETWEEN HUMIDITY MEASURING

int Humidity;
int Potentiometer;
int LightTimes=0;

void setup() {
  
  pinMode(2,OUTPUT);  //PUMP MOSFET
  pinMode(3,OUTPUT);  //LAMP MOSFET
  pinMode(4,OUTPUT);  //HUMIDITY SENSOR POWER SUPPLY
  pinMode(A1,INPUT);  //HUMIDITY SENSOR INDICATION
  pinMode(A2,INPUT_PULLUP);  //MIN HUMIDITY LEVEL SETTING (POTENTIOMETER)
  pinMode(A3,INPUT);  //WATER TANK LEVEL SENSOR
  analogReference(DEFAULT);
  
  lcd.init();                  
  lcd.backlight();
  
  light(true); //LAMP SWITCHING ON 

  Humidity=humidityINIT();
  Potentiometer=potentiometer();
  
  //Serial.begin(9600); //FOR TESTS
  
  }

void loop() {
  if (LightON.isReady()) {
    LightTimes=LightTimes+1;
    light(false);
    LightON.stop();
    LightOFF.start();
  }
  
  if (LightOFF.isReady()&&LightTimes<8) {
    light(true);
    LightOFF.stop();
    LightON.start();
  }
  
  if (hours24.isReady()) {
    light(true);
    LightTimes=0;
    LightON.start();
  }
  
  if (HumidityTime.isReady()) {
    digitalWrite(4, HIGH);
    delay(3000);
    HumidityTime.start();
    Humidity=humidity();
    Potentiometer=potentiometer();
    if (Humidity<Potentiometer) {
      if (midArifm()>4.95)
       pump();
    }
    digitalWrite(4, LOW);
  }                                  

  Display(potentiometer(),Humidity); 
   //Serial.println(Humidity); //FOR TESTS
  
}

void pump() {
  digitalWrite(2, HIGH);
  delay (15000);
  digitalWrite(2, LOW);
  
}

// WATER TANK LEVEL SENSOR FILTER
float midArifm() { 
  float sum = 0;                    
  for (int i = 0; i < 100; i++){  
    sum += (float)(analogRead(A3) * 5.0) / 1025;}
    delay(5000);
    return (sum / 100);
}

//HUMIDITY SENSOR INDICATION
int humidity() { 
  float humidityVoltage = (float)(analogRead(A1) * 5.0) / 1025;  
  int humidityValue = constrain(map(humidityVoltage*100, 300, 200, 0, 100), 0, 99); ; //to %
  delay(1);      
  return humidityValue;}

int humidityINIT() {
  digitalWrite(4, HIGH);
  delay (3000);
  int Humidity=humidity();
  digitalWrite(4, LOW);
  return Humidity;
}

//MIN HUMIDITY LEVEL SETTING (POTENTIOMETER)
int potentiometer(){  
  float potentiometerVoltage = (float)(analogRead(A2) * 5.0) / 1025; 
  int potentiometerValue = constrain(map(potentiometerVoltage*100, 14, 500, 0, 100), 0, 99); ; //to %
  delay(1);        
  return potentiometerValue;}

void Display(int Value1,int Value2){
  
  lcd.setCursor(0, 0);                //HUMIDITY SET
  lcd.print("Humidity set:");
  if (Value1>=10) {
    lcd.setCursor(13, 0);
    lcd.print(Value1);}
  if (Value1<10) {
    lcd.print(" ");
    lcd.setCursor(14, 0);
    lcd.print(Value1);}
  lcd.setCursor(15, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);                //HUMIDITY SENSOR 
  lcd.print("Humid. soil:");
  if (Value1>=10) {
    lcd.setCursor(13, 1);
    lcd.print(Value2);}
  if (Value2<10) {
    lcd.print(" ");
    lcd.setCursor(14, 1);
    lcd.print(Value2);}
  lcd.setCursor(15, 1);
  lcd.print("%");}


void light(boolean state)  { 
  if (state)digitalWrite(3, HIGH);
  else digitalWrite(3, LOW);}
