#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(22,21,12,14,27,26,5,18,23,19);

const byte ROWS = 3; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}
};

byte rowPins[ROWS] = {25, 33, 32};
byte colPins[COLS] = {15, 2, 0};

Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

enum DisplayMode { NONE, STRIKE, DIP, DIP_DIRECTION, GPS, BATTERY };
DisplayMode currentMode = NONE;

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 500;

bool paused = false;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Connecting...");
  delay(1000);
  lcd.clear();
  lcd.print("Connected");
  delay(1000);
  lcd.clear();
}

void loop() {
  char key = keypad.getKey();
  
  // Update mode based on key press
  if (key == '1') currentMode = STRIKE;
  if (key == '2') currentMode = DIP;
  if (key == '3') currentMode = DIP_DIRECTION;
  if (key == '4') currentMode = GPS;
  if (key == '5') currentMode = BATTERY;
  if (key == '9') paused = !paused;

  // Update and display based on mode
  if (!paused && millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();

    int strike_value = random(0, 360);
    int dip_value = random(0, 90);
    int dip_direction_value = (strike_value + 90) % 360;

    float latitude = 37.774;  
    float longitude = -122.419;

    String gps_string = String(latitude, 3) + "," + String(longitude, 3);

    String battery = "97%";

    lcd.clear();
    if (currentMode == STRIKE) {
      lcd.setCursor(0, 0);				
      lcd.print("Strike:");
      lcd.setCursor(0, 1);
      lcd.print(strike_value);
    } else if (currentMode == DIP) {
      lcd.setCursor(0, 0);				
      lcd.print("Dip:");
      lcd.setCursor(0, 1);
      lcd.print(dip_value);
    } else if (currentMode == DIP_DIRECTION) {
      lcd.setCursor(0, 0);				
      lcd.print("Dip Direction:");
      lcd.setCursor(0, 1);
      lcd.print(dip_direction_value);
    } else if (currentMode == GPS) {
      lcd.setCursor(0, 0);				
      lcd.print("GPS:");
      lcd.setCursor(0, 1);
      lcd.print(gps_string);
    }else if (currentMode == GPS) {
      lcd.setCursor(0, 0);				
      lcd.print("GPS:");
      lcd.setCursor(0, 1);
      lcd.print(gps_string);
    } else if (currentMode == BATTERY) {
      lcd.setCursor(0, 0);				
      lcd.print("Battery:");
      lcd.setCursor(0, 1);
      lcd.print(battery);
    }
  }
}
