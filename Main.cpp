#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int PreviousVal=0;
char CLEAR[] = "               ";

int enable1Pin = 9;
int speed = 50;
int speed2 = 255;
float TEMP_LOW = 25;
float TEMP_HIGH = 34;
int PWR_LOW = 0;
int PWR_HIGH = 255;

void setup() {

 lcd.init(); // initialize the lcd
 lcd.backlight();
 lcd.print("Temp:      C"); //Del?

  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(enable1Pin, OUTPUT);

  int reps = 4;
  long sum = 0;
  for (int i = 0; i < reps; i++) {
    sum += analogRead(A0);
    Serial.println(sum);
  }
  sum = sum / reps;
  TEMP_LOW = toCelsius(sum);
  TEMP_HIGH = TEMP_LOW + 8;
  Serial.println(TEMP_LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int adcVal = analogRead(A0);
  float tempC = toCelsius(adcVal);

  Serial.print(tempC);
  Serial.print("\t");
  Serial.print(TEMP_LOW);
  Serial.print("\t");
  int mappedVal = map(tempC, TEMP_LOW, TEMP_HIGH, PWR_LOW, PWR_HIGH);
  int revVal = 255 - mappedVal;
  analogWrite(enable1Pin, revVal);
  Serial.println(mappedVal);
  displayTemp(tempC, mappedVal);
  delay(500);
/*
  lcd.print(tempC);
lcd.print("C");
lcd.setCursor(0,1);
if (mappedVal<=75){

lcd.print("Brrr!!!");
lcd.print(CLEAR)
}
else if (mappedVal>75 && mappedVal<=135 ){
  lcd.print("It's chill");
  lcd.print(CLEAR)
}
else if (mappedVal>135 && mappedVal<=195){
  lcd.print("Perfect!");
  lcd.print(CLEAR)
}
else if (mappedVal>195){
  lcd.print("Hot");
  lcd.print(CLEAR)
}
lcd.setCursor(5,0);
*/
}

float toCelsius(long reading) {
  float v = reading * 5.0 / 1024;
  float Rt = 10 * v / (5 - v);
  float tempK = 1 / (log(Rt / 10) / 3950 + 1 / (273.15 + 25));
  float tempC = tempK - 273.15;
  return tempC;
}

void displayTemp(float temp, int pwrVal) {
  lcd.setCursor(5, 0);
  lcd.print(temp);
  lcd.print(" ");

  lcd.setCursor(0,1);
  if (pwrVal<=75){
    lcd.print("Brrr!!!");
  }
  else if (pwrVal<=135 ){
    lcd.print("It's Chill!");
  }
  else if (pwrVal<=195){
    lcd.print("Perfect!");
  }
  else if (pwrVal>195){
    lcd.print("Hot!");
  }
  lcd.print(CLEAR);
}
