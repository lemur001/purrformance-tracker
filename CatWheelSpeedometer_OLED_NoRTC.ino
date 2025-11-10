/*
 * Cat Wheel Speedometer & Odometer - OLED Version (No RTC - Testing)
 *
 * This version removes RTC to test display and hall sensor only.
 * Day/night tracking and midnight reset disabled.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==================== PIN DEFINITIONS ====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // Try 0x3D if display doesn't work

// Sensor and Button Pins
#define HALL_SENSOR_PIN    5    // Hall effect sensor input
#define RESET_ALL_BTN      6    // Hold 3 seconds to reset all stats

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
const unsigned long RESET_HOLD_TIME = 3000;

// ==================== GLOBAL VARIABLES ====================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Speed and Distance Tracking
unsigned long lastMagnetTime = 0;
unsigned long lastValidMagnetTime = 0;
float currentSpeed = 0.0;
int magnetCounter = 0;

// Stats (stored in RAM only)
float todayDistance = 0.0;
float lifetimeTopSpeed = 0.0;
float lifetimeTotalMiles = 0.0;

// Hall Sensor State
bool lastHallState = HIGH;

// Button State
bool lastResetState = HIGH;
unsigned long resetPressStartTime = 0;
bool resetInProgress = false;

// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  // Initialize pins
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RESET_ALL_BTN, INPUT_PULLUP);

  // Initialize OLED Display
  Serial.println("Initializing display...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Display FAILED!");
    Serial.println("Try changing SCREEN_ADDRESS to 0x3D");
    while(1);
  }

  Serial.println("Display SUCCESS!");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 28);
  display.println(F("Initializing..."));
  display.display();
  delay(2000);

  Serial.println("Drawing display...");
  updateDisplay();
  Serial.println("Ready!");
}

// ==================== MAIN LOOP ====================
void loop() {
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

// ==================== RESET FUNCTION ====================
void resetAllStats() {
  lifetimeTopSpeed = 0.0;
  lifetimeTotalMiles = 0.0;
  todayDistance = 0.0;
  magnetCounter = 0;
}

// ==================== DISPLAY FUNCTION ====================
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Line 0: "----------DAILY-----------"
  display.setCursor(0, 0);
  display.println(F("----------DAILY----------"));

  // Line 1: Speed - left label, right-aligned value
  display.setCursor(0, 8);
  display.print(F("Speed:"));
  char speedStr[9];
  dtostrf(currentSpeed, 7, 1, speedStr);
  display.setCursor(68, 8);
  display.print(speedStr);
  display.print(F(" MPH"));

  // Line 2: Distance - left label, right-aligned value
  display.setCursor(0, 16);
  display.print(F("Distance:"));
  char todayStr[7];
  dtostrf(todayDistance, 6, 1, todayStr);
  display.setCursor(74, 16);
  display.print(todayStr);
  display.print(F(" mi"));

  // Line 3: "-----HIGH SCORES-----"
  display.setCursor(0, 24);
  display.println(F("-----HIGH SCORES-----"));

  // Line 4: Top Speed - left label, right-aligned value
  display.setCursor(0, 32);
  display.print(F("Top Speed:"));
  char topStr[7];
  dtostrf(lifetimeTopSpeed, 6, 1, topStr);
  display.setCursor(68, 32);
  display.print(topStr);
  display.print(F(" MPH"));

  // Line 5: Total Distance - left label, right-aligned value
  display.setCursor(0, 40);
  display.print(F("Total Dist:"));
  char totalStr[7];
  dtostrf(lifetimeTotalMiles, 6, 1, totalStr);
  display.setCursor(68, 40);
  display.print(totalStr);
  display.print(F(" mi"));

  // Line 6-7: Note about no RTC
  display.setCursor(0, 50);
  display.println(F("(No RTC - test mode)"));

  display.display();
}
