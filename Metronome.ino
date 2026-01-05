// Byakuroku Metronome Final Version (Foam Optimized)
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
const int buzzerPin = 8;
const int buttonPin = 2;

volatile int mode = 0; 
volatile bool modeChanged = false; 
unsigned long lastDebounceTime = 0;
unsigned long lastStepTime = 0; 
int currentStep = 0; 
int lastBpm = -1;
int lastModeDisplay = -1;
int systemState = 0; 

byte bigCircle[8] = {0b01110,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b01110}; 
byte midCircle[8] = {0b00000,0b01110,0b01110,0b01110,0b01110,0b01110,0b00000,0b00000}; 

void muffledClick(int intensity, int delayTime) {
  for (int i = 0; i < intensity; i++) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(100); 
  }
}

void switchMode() {
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) > 350) { 
    mode = (mode + 1) % 2;
    modeChanged = true; 
    lastDebounceTime = currentTime;
  }
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), switchMode, FALLING);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, bigCircle);
  lcd.createChar(1, midCircle);
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(A0);

  // --- 三段式開關邏輯 ---
  if (sensorValue < 80) { 
    if (systemState != 2) {
      lcd.noBacklight(); lcd.clear();
      lcd.setCursor(4, 0); lcd.print("SLEEPING...");
      systemState = 2;
    }
    return; 
  } 
  else if (sensorValue < 220) {
    if (systemState != 1) {
      lcd.backlight(); lcd.clear();
      lcd.setCursor(0, 0); lcd.print(mode == 0 ? "CLA" : "GUI");
      lcd.setCursor(9, 0); lcd.print("BPM:OFF?"); 
      lcd.setCursor(0, 1); lcd.print("  <<< TURN OFF  ");
      systemState = 1;
    }
    return;
  }
  else {
    if (systemState != 0) {
      lcd.backlight(); lcd.clear();
      systemState = 0; modeChanged = true; lastBpm = -1;
    }
  }

  int bpm = map(sensorValue, 221, 1023, 60, 180); 
  unsigned long stepDuration = 30000 / bpm; 

  if (modeChanged) {
    currentStep = 0;
    lcd.setCursor(0, 1); lcd.print("                ");
    modeChanged = false;
  }

  if (abs(bpm - lastBpm) > 1 || mode != lastModeDisplay) {
    lcd.setCursor(0, 0);
    lcd.print(mode == 0 ? "CLA" : "GUI");
    lcd.setCursor(9, 0); 
    lcd.print("BPM:");
    if(bpm < 100) lcd.print(" ");
    lcd.print(bpm);
    lcd.setCursor(3, 0); lcd.print("      "); 
    lastBpm = bpm;
    lastModeDisplay = mode;
  }

  if (millis() - lastStepTime >= stepDuration) {
    lastStepTime = millis();
    
    // --- 核心音效數據 ---
    if (mode == 1) { // GUI
      if (currentStep == 0) muffledClick(1, 1200); // 重音砰
      else if (currentStep == 2 || currentStep == 6) muffledClick(1, 1000); // 掐
      else muffledClick(2, 1200); // 輕砰
    } else { // CLA
      if (currentStep % 2 == 0) muffledClick(currentStep == 0 ? 3 : 1, 1200);
    }

    // --- 核心視覺數據 ---
    if (currentStep == 0) { lcd.setCursor(0, 1); lcd.print("                "); }
    if (mode == 0) { 
      if (currentStep % 2 == 0) {
        int beat = currentStep / 2;
        lcd.setCursor(beat * 4 + 1, 1); 
        if (beat == 0) { lcd.write(0); lcd.write(0); }
        else           { lcd.write(1); lcd.write(1); }
      }
    } else { 
      lcd.setCursor(currentStep * 2, 1);
      if (currentStep == 2 || currentStep == 6) { lcd.write(1); lcd.write(1); }
      else { lcd.write(0); lcd.write(0); }
    }
    currentStep = (currentStep + 1) % 8;
  }
}