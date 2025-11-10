/*
 * Cat Wheel Speedometer & Odometer - OLED Version
 *
 * Hardware:
 * - Adafruit SSD1306 OLED Display (128x64, I2C)
 * - DS3231 RTC Module (I2C) with CR2032 battery
 * - Hall Effect Sensor (G R Y pins)
 * - 1 Reset Button (hold 3 seconds to reset all)
 * - Arduino Mega 2560
 *
 * Wheel Specifications:
 * - Circumference: 118 inches
 * - Diameter: 37.6 inches (118 / π)
 * - Distance per magnet: 9.83 inches
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

// ==================== PIN DEFINITIONS ====================
// OLED Display (I2C) - shares I2C bus with RTC
// SDA = D20 (Mega 2560)
// SCL = D21 (Mega 2560)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // Try 0x3D if display doesn't work

// Sensor and Button Pins
#define HALL_SENSOR_PIN    5    // Hall effect sensor input
#define RESET_ALL_BTN      6    // Hold 3 seconds to reset all stats

// Location for Day/Night Calculation (Chicago, IL)
const float LATITUDE = 41.8781;
const float LONGITUDE = -87.6298;

// ==================== CONSTANTS ====================
const float WHEEL_CIRCUMFERENCE = 118.0;
const float WHEEL_DIAMETER = WHEEL_CIRCUMFERENCE / 3.14159265359;
const int MAGNET_COUNT = 12;
const float DISTANCE_PER_MAGNET = WHEEL_CIRCUMFERENCE / MAGNET_COUNT;
const float INCHES_PER_MILE = 63360.0;

// Timing and Validation
const unsigned long SPEED_TIMEOUT = 4000;          // 4 sec timeout
const unsigned long MIN_MAGNET_INTERVAL = 45;      // 45ms debounce
const float MAX_CAT_SPEED = 35.0;                  // 35 MPH max
const unsigned long RESET_HOLD_TIME = 3000;        // 3 sec hold to reset
const unsigned long PEAK_HOLD_TIME = 30000;        // 30 sec peak speed hold

// ==================== GLOBAL VARIABLES ====================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

// Speed and Distance Tracking
unsigned long lastMagnetTime = 0;
unsigned long lastValidMagnetTime = 0;
float currentSpeed = 0.0;
float peakSpeed = 0.0;                  // Peak speed (holds for 30 sec)
unsigned long peakSpeedTime = 0;        // When peak was set
int magnetCounter = 0;

// Stats (stored in RAM only - no SD card)
float todayDistance = 0.0;
float dayMiles = 0.0;
float nightMiles = 0.0;
float lifetimeTopSpeed = 0.0;
float lifetimeTotalMiles = 0.0;

// Date tracking
uint8_t lastDay = 0;

// Hall Sensor State
bool lastHallState = HIGH;

// Button State
bool lastResetState = HIGH;
unsigned long resetPressStartTime = 0;
bool resetInProgress = false;

// ==================== SETUP ====================
void setup() {
  // Initialize pins
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RESET_ALL_BTN, INPUT_PULLUP);

  // Initialize OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while(1); // Display failed - halt
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
    // Set to November 10, 2025 at 12:20 AM
    rtc.adjust(DateTime(2025, 11, 10, 0, 20, 0));
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

  // Check for peak speed timeout (30 seconds)
  if (millis() - peakSpeedTime > PEAK_HOLD_TIME && peakSpeed != 0.0) {
    peakSpeed = 0.0;
    updateDisplay();
  }

  // Check reset button (hold 3 seconds)
  handleResetButton();

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

      // Update peak speed if current is higher
      if (currentSpeed > peakSpeed) {
        peakSpeed = currentSpeed;
        peakSpeedTime = currentTime;
      }

      if (currentSpeed > lifetimeTopSpeed) {
        lifetimeTopSpeed = currentSpeed;
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
  }
}

// ==================== BUTTON HANDLING ====================
void handleResetButton() {
  bool resetState = digitalRead(RESET_ALL_BTN);
  unsigned long currentTime = millis();

  // Button just pressed
  if (resetState == LOW && lastResetState == HIGH) {
    resetPressStartTime = currentTime;
    resetInProgress = true;
  }

  // Button being held
  if (resetState == LOW && resetInProgress) {
    unsigned long holdDuration = currentTime - resetPressStartTime;

    // Held for 3 seconds - reset all stats
    if (holdDuration >= RESET_HOLD_TIME) {
      resetAllStats();
      resetInProgress = false;

      // Show confirmation message
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(25, 24);
      display.println(F("RESET!"));
      display.display();
      delay(1000);
      updateDisplay();
    }
  }

  // Button released
  if (resetState == HIGH && lastResetState == LOW) {
    resetInProgress = false;
  }

  lastResetState = resetState;
}

// ==================== DAY CHANGE DETECTION ====================
void checkDayChange() {
  DateTime now = rtc.now();
  if (now.day() != lastDay) {
    todayDistance = 0.0;
    lastDay = now.day();
    updateDisplay();
  }
}

// ==================== RESET FUNCTION ====================
void resetAllStats() {
  lifetimeTopSpeed = 0.0;
  lifetimeTotalMiles = 0.0;
  dayMiles = 0.0;
  nightMiles = 0.0;
  todayDistance = 0.0;
  peakSpeed = 0.0;
  magnetCounter = 0;
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

// ==================== DISPLAY FUNCTION ====================
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Line 0: Speed ("  0.0 MPH" right-aligned to pixel 127)
  display.setCursor(0, 0);
  display.print(F("Speed:"));
  char speedStr[6];
  dtostrf(currentSpeed, 5, 1, speedStr);
  display.setCursor(74, 0);
  display.print(speedStr);
  display.setCursor(110, 0);
  display.print(F("MPH"));

  // Line 1: Peak Speed (aligned with Speed above)
  char peakStr[6];
  dtostrf(peakSpeed, 5, 1, peakStr);
  display.setCursor(74, 8);
  display.print(peakStr);
  display.setCursor(110, 8);
  display.print(F("MPH"));

  // Line 2: Distance (" 0.00 mi" right-aligned to pixel 127)
  display.setCursor(0, 16);
  display.print(F("Distance:"));
  char todayStr[6];
  dtostrf(todayDistance, 5, 2, todayStr);
  display.setCursor(80, 16);
  display.print(todayStr);
  display.setCursor(116, 16);
  display.print(F("mi"));

  // Line 3: "-----HIGH SCORES-----"
  display.setCursor(0, 24);
  display.println(F("-----HIGH SCORES-----"));

  // Line 4: Top Speed (aligned with Speed/Peak above)
  display.setCursor(0, 32);
  display.print(F("Top Speed:"));
  char topStr[6];
  dtostrf(lifetimeTopSpeed, 5, 1, topStr);
  display.setCursor(74, 32);
  display.print(topStr);
  display.setCursor(110, 32);
  display.print(F("MPH"));

  // Line 5: Total Distance (aligned with Distance above)
  display.setCursor(0, 40);
  display.print(F("Total Dist:"));
  char totalStr[6];
  dtostrf(lifetimeTotalMiles, 5, 2, totalStr);
  display.setCursor(80, 40);
  display.print(totalStr);
  display.setCursor(116, 40);
  display.print(F("mi"));

  // Line 6: Daytime (aligned with distances above)
  display.setCursor(0, 48);
  display.print(F("Daytime:"));
  char dayStr[6];
  dtostrf(dayMiles, 5, 2, dayStr);
  display.setCursor(80, 48);
  display.print(dayStr);
  display.setCursor(116, 48);
  display.print(F("mi"));

  // Line 7: Nighttime (aligned with distances above)
  display.setCursor(0, 56);
  display.print(F("Nighttime:"));
  char nightStr[6];
  dtostrf(nightMiles, 5, 2, nightStr);
  display.setCursor(80, 56);
  display.print(nightStr);
  display.setCursor(116, 56);
  display.print(F("mi"));

  display.display();
}
