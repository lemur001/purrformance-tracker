/*
 * Cat Wheel Speedometer & Odometer - OLED Version
 *
 * Hardware:
 * - Adafruit SSD1306 OLED Display (128x64, I2C)
 * - DS3231 or DS1307 RTC Module (I2C)
 * - Hall Effect Sensor
 * - 12 Magnets on cat wheel
 * - Arduino Mega 2560
 *
 * Wheel Specifications:
 * - Diameter: 118 inches
 * - Circumference: 370.7 inches
 * - Distance per magnet: 30.89 inches
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

// ==================== PIN DEFINITIONS ====================
// OLED Display (I2C) - shares I2C bus with RTC
// SDA = D20 (Mega 2560)
// SCL = D21 (Mega 2560)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // No reset pin
#define SCREEN_ADDRESS 0x3C  // Common I2C address (try 0x3D if this doesn't work)

// SD Card Pin
#define SD_CS     4     // SD Card Chip Select
// SD uses hardware SPI: MOSI = 51, SCK = 52, MISO = 50

// Sensor and Button Pins
#define HALL_SENSOR_PIN    5    // Hall effect sensor input
#define TODAY_RESET_BTN    6    // Button to reset today's miles
#define ALL_RESET_BTN      7    // Button to reset all high scores

// Location for Day/Night Calculation (Chicago, IL)
const float LATITUDE = 41.8781;
const float LONGITUDE = -87.6298;
const int TIMEZONE_OFFSET = -6;

// ==================== CONSTANTS ====================
const float WHEEL_DIAMETER = 118.0;
const float WHEEL_CIRCUMFERENCE = 3.14159265359 * WHEEL_DIAMETER;
const int MAGNET_COUNT = 12;
const float DISTANCE_PER_MAGNET = WHEEL_CIRCUMFERENCE / MAGNET_COUNT;
const float INCHES_PER_MILE = 63360.0;

// Timing and Validation
const unsigned long SPEED_TIMEOUT = 4000;
const unsigned long MIN_MAGNET_INTERVAL = 45;
const float MAX_CAT_SPEED = 35.0;
const unsigned long DEBOUNCE_DELAY = 10;

// ==================== GLOBAL VARIABLES ====================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

// Speed and Distance Tracking
unsigned long lastMagnetTime = 0;
unsigned long lastValidMagnetTime = 0;
float currentSpeed = 0.0;
int magnetCounter = 0;

// Stats
float todayDistance = 0.0;
float dayMiles = 0.0;
float nightMiles = 0.0;
float lifetimeTopSpeed = 0.0;
float lifetimeTotalMiles = 0.0;

// Date tracking
uint8_t lastDay = 0;

// Hall Sensor State
bool lastHallState = HIGH;

// Button States
bool lastTodayResetState = HIGH;
bool lastAllResetState = HIGH;
unsigned long lastTodayResetDebounce = 0;
unsigned long lastAllResetDebounce = 0;

// ==================== SETUP ====================
void setup() {
  // Initialize pins
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  pinMode(TODAY_RESET_BTN, INPUT_PULLUP);
  pinMode(ALL_RESET_BTN, INPUT_PULLUP);

  // Initialize OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Display failed - halt
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 28);
  display.println(F("Initializing..."));
  display.display();
  delay(1000);

  // Initialize RTC
  if (!rtc.begin()) {
    display.clearDisplay();
    display.setCursor(10, 28);
    display.println(F("RTC Error!"));
    display.display();
    while (1) delay(1000);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(2024, 11, 10, 12, 0, 0));
  }

  // Initialize SD Card
  if (SD.begin(SD_CS)) {
    loadDataFromSD();
  }

  // Get current day for tracking
  DateTime now = rtc.now();
  lastDay = now.day();

  delay(1000);

  // Draw initial display
  updateDisplay();
}

// ==================== MAIN LOOP ====================
void loop() {
  // Check for day change
  checkDayChange();

  // Read hall sensor
  bool currentHallState = digitalRead(HALL_SENSOR_PIN);

  // Detect magnet pass
  if (lastHallState == HIGH && currentHallState == LOW) {
    handleMagnetDetection();
  }

  lastHallState = currentHallState;

  // Check for speed timeout
  if (millis() - lastValidMagnetTime > SPEED_TIMEOUT && currentSpeed != 0.0) {
    currentSpeed = 0.0;
    updateDisplay();
  }

  // Check buttons
  handleButtons();

  delay(10);
}

// ==================== MAGNET DETECTION ====================
void handleMagnetDetection() {
  unsigned long currentTime = millis();
  unsigned long timeSinceLastMagnet = currentTime - lastMagnetTime;

  if (timeSinceLastMagnet < MIN_MAGNET_INTERVAL) {
    return;
  }

  lastMagnetTime = currentTime;

  if (lastValidMagnetTime > 0) {
    unsigned long timeDiff = currentTime - lastValidMagnetTime;
    float calculatedSpeed = (DISTANCE_PER_MAGNET / timeDiff) * 3600000.0 / INCHES_PER_MILE;

    if (calculatedSpeed <= MAX_CAT_SPEED) {
      currentSpeed = calculatedSpeed;
      lastValidMagnetTime = currentTime;

      if (currentSpeed > lifetimeTopSpeed) {
        lifetimeTopSpeed = currentSpeed;
        saveDataToSD();
      }

      updateDisplay();
    } else {
      return;
    }
  } else {
    lastValidMagnetTime = currentTime;
  }

  // Update distance
  magnetCounter++;
  if (magnetCounter >= MAGNET_COUNT) {
    float rotationDistance = WHEEL_CIRCUMFERENCE / INCHES_PER_MILE;

    todayDistance += rotationDistance;

    if (isDaytime()) {
      dayMiles += rotationDistance;
    } else {
      nightMiles += rotationDistance;
    }

    lifetimeTotalMiles += rotationDistance;
    magnetCounter = 0;

    updateDisplay();
    saveDataToSD();
  }
}

// ==================== BUTTON HANDLING ====================
void handleButtons() {
  unsigned long currentTime = millis();

  // Today Reset Button
  bool todayResetState = digitalRead(TODAY_RESET_BTN);
  if (todayResetState != lastTodayResetState) {
    lastTodayResetDebounce = currentTime;
  }
  if ((currentTime - lastTodayResetDebounce) > DEBOUNCE_DELAY) {
    if (todayResetState == LOW && lastTodayResetState == HIGH) {
      resetTodayStats();
    }
  }
  lastTodayResetState = todayResetState;

  // All Reset Button
  bool allResetState = digitalRead(ALL_RESET_BTN);
  if (allResetState != lastAllResetState) {
    lastAllResetDebounce = currentTime;
  }
  if ((currentTime - lastAllResetDebounce) > DEBOUNCE_DELAY) {
    if (allResetState == LOW && lastAllResetState == HIGH) {
      resetAllStats();
    }
  }
  lastAllResetState = allResetState;
}

// ==================== DAY CHANGE DETECTION ====================
void checkDayChange() {
  DateTime now = rtc.now();
  if (now.day() != lastDay) {
    todayDistance = 0.0;
    lastDay = now.day();
    updateDisplay();
    saveDataToSD();
  }
}

// ==================== RESET FUNCTIONS ====================
void resetTodayStats() {
  todayDistance = 0.0;
  magnetCounter = 0;
  updateDisplay();
  saveDataToSD();
}

void resetAllStats() {
  lifetimeTopSpeed = 0.0;
  lifetimeTotalMiles = 0.0;
  dayMiles = 0.0;
  nightMiles = 0.0;
  todayDistance = 0.0;
  magnetCounter = 0;
  updateDisplay();
  saveDataToSD();
}

// ==================== DAY/NIGHT CALCULATION ====================
bool isDaytime() {
  DateTime now = rtc.now();
  int dayOfYear = getDayOfYear(now.year(), now.month(), now.day());
  float sunriseHour = calculateSunrise(dayOfYear);
  float sunsetHour = calculateSunset(dayOfYear);
  float currentHour = now.hour() + now.minute() / 60.0;
  return (currentHour >= sunriseHour && currentHour < sunsetHour);
}

int getDayOfYear(int year, int month, int day) {
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
    daysInMonth[1] = 29;
  }
  int dayOfYear = day;
  for (int i = 0; i < month - 1; i++) {
    dayOfYear += daysInMonth[i];
  }
  return dayOfYear;
}

float calculateSunrise(int dayOfYear) {
  float amplitude = 2.5;
  float offset = 6.3;
  float phase = (dayOfYear - 172) * 2.0 * 3.14159 / 365.0;
  return offset - amplitude * cos(phase);
}

float calculateSunset(int dayOfYear) {
  float amplitude = 2.5;
  float offset = 18.5;
  float phase = (dayOfYear - 172) * 2.0 * 3.14159 / 365.0;
  return offset + amplitude * cos(phase);
}

// ==================== SD CARD FUNCTIONS ====================
void loadDataFromSD() {
  File dataFile = SD.open("catwheel.txt", FILE_READ);
  if (dataFile) {
    String line = dataFile.readStringUntil('\n');
    dataFile.close();

    int commas[4];
    int commaCount = 0;
    for (int i = 0; i < line.length() && commaCount < 4; i++) {
      if (line.charAt(i) == ',') {
        commas[commaCount++] = i;
      }
    }

    if (commaCount == 4) {
      todayDistance = line.substring(0, commas[0]).toFloat();
      dayMiles = line.substring(commas[0] + 1, commas[1]).toFloat();
      nightMiles = line.substring(commas[1] + 1, commas[2]).toFloat();
      lifetimeTopSpeed = line.substring(commas[2] + 1, commas[3]).toFloat();
      lifetimeTotalMiles = line.substring(commas[3] + 1).toFloat();
    }
  }
}

void saveDataToSD() {
  if (SD.exists("catwheel.txt")) {
    SD.remove("catwheel.txt");
  }
  File dataFile = SD.open("catwheel.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(todayDistance, 1);
    dataFile.print(',');
    dataFile.print(dayMiles, 1);
    dataFile.print(',');
    dataFile.print(nightMiles, 1);
    dataFile.print(',');
    dataFile.print(lifetimeTopSpeed, 1);
    dataFile.print(',');
    dataFile.println(lifetimeTotalMiles, 1);
    dataFile.close();
  }
}

// ==================== DISPLAY FUNCTION ====================
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Line 1: Current Speed (large)
  display.setCursor(0, 0);
  display.print(F("Speed:"));
  display.setCursor(42, 0);
  display.setTextSize(2);
  char speedStr[7];
  dtostrf(currentSpeed, 4, 1, speedStr);
  display.print(speedStr);
  display.setTextSize(1);
  display.print(F("MPH"));

  // Line 2: Miles Today
  display.setCursor(0, 16);
  display.print(F("Today:"));
  char todayStr[7];
  dtostrf(todayDistance, 5, 1, todayStr);
  display.setCursor(42, 16);
  display.print(todayStr);
  display.print(F(" mi"));

  // Line 3: Separator
  display.setCursor(0, 24);
  display.println(F("---HIGH SCORES---"));

  // Line 4: Top Speed
  display.setCursor(0, 32);
  display.print(F("Top:  "));
  char topStr[7];
  dtostrf(lifetimeTopSpeed, 4, 1, topStr);
  display.print(topStr);
  display.print(F(" MPH"));

  // Line 5: Total Miles
  display.setCursor(0, 40);
  display.print(F("Total:"));
  char totalStr[7];
  dtostrf(lifetimeTotalMiles, 5, 1, totalStr);
  display.print(totalStr);
  display.print(F(" mi"));

  // Line 6: Day Miles
  display.setCursor(0, 48);
  display.print(F("Day:  "));
  char dayStr[7];
  dtostrf(dayMiles, 5, 1, dayStr);
  display.print(dayStr);
  display.print(F(" mi"));

  // Line 7: Night Miles
  display.setCursor(0, 56);
  display.print(F("Night:"));
  char nightStr[7];
  dtostrf(nightMiles, 5, 1, nightStr);
  display.print(nightStr);
  display.print(F(" mi"));

  display.display();
}
