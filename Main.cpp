#include <LiquidCrystal_I2C.h>
#include "analogWave.h"

// Note frequencies in Hz
#define REST 0
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

LiquidCrystal_I2C lcd(0x27, 16, 2);
int PreviousVal = 0;
char CLEAR[] = "               ";

int enable1Pin = 9;

float TEMP_LOW = 25;
float TEMP_HIGH = 34;
int PWR_LOW = 0;
int PWR_HIGH = 255;

// Melody and rhythm defined as an array
// Each note is followed by its duration (4 = quarter note, 8 = eighth note, etc.)
int melody[] = {
 // Super Mario Bros theme
 // Score available at https://musescore.com/user/2123/scores/2145
 // Theme by Koji Kondo
 NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
 NOTE_G5,4, REST,4, NOTE_G4,8, REST,4,
 NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
 NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
 NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
 REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
};
analogWave wave(DAC); // Initialize the analogWave object for DAC(A0)
int noteCounter = 0; // Index to keep track of which note is being played
int tempo = 200; // Set the tempo in BPM (Beats Per Minute)
int wholenote = (60000 * 4) / tempo; // Calculate the duration of a whole note in ms
int divider = 0, noteDuration = 0; // Variables to hold note duration

unsigned long noteStart = 0, currMillis = 0;

void setup() {
 lcd.init(); // initialize the lcd
 lcd.backlight();
 lcd.print("Temp:"); //Del?

  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(enable1Pin, OUTPUT);
  wave.sine(10);

  int reps = 4;
  long sum = 0;
  for (int i = 0; i < reps; i++) {
    sum += analogRead(A1);
    Serial.println(sum);
  }
  sum = sum / reps;
  TEMP_LOW = toCelsius(sum);
  TEMP_HIGH = TEMP_LOW + 8;
  Serial.println(TEMP_LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int adcVal = analogRead(A1);
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
  playMelody();
  delay(100);
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
  lcd.print(" C");
  lcd.print(CLEAR);

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

void playMelody() {
  // Calculate the duration of the current note
  divider = melody[noteCounter + 1];
  if (divider > 0) {
    // For regular notes
    noteDuration = wholenote / divider;
  } else if (divider < 0) {
    // For dotted notes (duration increased by 50%)
    noteDuration = wholenote / abs(divider);
    noteDuration *= 1.5; // Increase the duration by 50% for dotted notes
  }
  // Play the note
  wave.freq(melody[noteCounter]);
  currMillis = millis();
  if (currMillis - noteStart >= noteDuration * 0.85) {
    wave.stop();
  }
  /*
  delay(noteDuration * 0.85); // Play the note for 85% of its duration
  wave.stop();
  */
  if (currMillis - noteStart >= noteDuration) {
    // Increment the note counter by 2 (because each note is followed by its duration)
    noteCounter += 2;
    // Reset the counter when reaching the end of the melody
    int totalNotes = sizeof(melody) / sizeof(melody[0]);
    noteCounter = noteCounter % totalNotes;
    noteStart = currMillis;
  }
  /*
  // Pause between notes
  delay(noteDuration * 0.15); // Pause for 15% of the note duration
  // Increment the note counter by 2 (because each note is followed by its duration)
  noteCounter += 2;
  // Reset the counter when reaching the end of the melody
  int totalNotes = sizeof(melody) / sizeof(melody[0]);
  noteCounter = noteCounter % totalNotes;
  // Add a delay between repetitions of the melody
  if (noteCounter == 0) {
    delay(1000);
  }
  */
}
