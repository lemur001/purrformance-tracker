/*
 * Cat Wheel Speedometer & Odometer
 *
 * Hardware:
 * - 3.2" TFT SPI 240x320 v1.0 Display with SD Card
 * - DS3231 or DS1307 RTC Module (I2C)
 * - Hall Effect Sensor
 * - 12 Magnets on cat wheel
 * - Arduino (Mega recommended for memory)
 *
 * Wheel Specifications:
 * - Diameter: 118 inches
 * - Circumference: 370.7 inches
 * - Distance per magnet: 30.89 inches
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

// ==================== PIN DEFINITIONS ====================
// TFT Display Pins (SPI)
#define TFT_CS    10    // Chip Select
#define TFT_DC    9     // Data/Command
#define TFT_RST   8     // Reset
// MOSI = 11, SCK = 13, MISO = 12 (hardware SPI)

// SD Card Pin
#define SD_CS     4     // SD Card Chip Select (different from TFT_CS)

// Sensor and Button Pins
#define HALL_SENSOR_PIN    5    // Hall effect sensor input
#define TODAY_RESET_BTN    6    // Button to reset today's miles
#define ALL_RESET_BTN      7    // Button to reset all high scores

// Location for Day/Night Calculation (Chicago, IL)
const float LATITUDE = 41.8781;   // Chicago latitude
const float LONGITUDE = -87.6298; // Chicago longitude
const int TIMEZONE_OFFSET = -6;   // CST offset (use -5 for CDT)

// ==================== CONSTANTS ====================
const float WHEEL_DIAMETER = 118.0;                      // inches
const float WHEEL_CIRCUMFERENCE = 3.14159265359 * WHEEL_DIAMETER; // ~370.7 inches
const int MAGNET_COUNT = 12;                             // Magnets per rotation
const float DISTANCE_PER_MAGNET = WHEEL_CIRCUMFERENCE / MAGNET_COUNT; // ~30.89 inches
const float INCHES_PER_MILE = 63360.0;                   // Conversion factor

// Timing and Validation
const unsigned long SPEED_TIMEOUT = 4000;                // 4 seconds - reset speed to 0
const unsigned long MIN_MAGNET_INTERVAL = 45;            // 45ms minimum (prevents false triggers)
const float MAX_CAT_SPEED = 35.0;                        // 35 MPH max (cats sprint ~30 MPH)
const unsigned long DEBOUNCE_DELAY = 10;                 // 10ms debounce for buttons

// Display Colors (RGB565 format)
#define COLOR_BG       0x0000  // Black
#define COLOR_TEXT     0xFFFF  // White
#define COLOR_TITLE    0x07FF  // Cyan
#define COLOR_SPEED    0xF800  // Red (current speed)
#define COLOR_DISTANCE 0x07E0  // Green
#define COLOR_DAILY    0xFFE0  // Yellow
#define COLOR_LABEL    0xCE79  // Light gray

// ==================== GLOBAL VARIABLES ====================
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
RTC_DS3231 rtc;  // Use RTC_DS1307 if you have DS1307 instead

// Speed and Distance Tracking
unsigned long lastMagnetTime = 0;
unsigned long lastValidMagnetTime = 0;
float currentSpeed = 0.0;           // Current speed in MPH
int magnetCounter = 0;              // Count magnets for distance calculation

// Stats (all stored on SD)
float todayDistance = 0.0;          // Miles today (resets at midnight)
float dayMiles = 0.0;               // Miles during daylight hours
float nightMiles = 0.0;             // Miles during nighttime hours
float lifetimeTopSpeed = 0.0;       // All-time top speed
float lifetimeTotalMiles = 0.0;     // All-time total miles

// Date tracking
uint8_t lastDay = 0;                // Track day changes for midnight reset

// Hall Sensor State
bool lastHallState = HIGH;

// Button States
bool lastTodayResetState = HIGH;
bool lastAllResetState = HIGH;
unsigned long lastTodayResetDebounce = 0;
unsigned long lastAllResetDebounce = 0;

// Display Update Flags
bool needsFullRedraw = true;

// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);
  Serial.println(F("Cat Wheel Speedometer Starting..."));

  // Initialize pins
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  pinMode(TODAY_RESET_BTN, INPUT_PULLUP);
  pinMode(ALL_RESET_BTN, INPUT_PULLUP);

  // Initialize TFT Display
  tft.begin();
  tft.setRotation(0);  // Portrait mode (240 wide x 320 tall)
  tft.fillScreen(COLOR_BG);

  // Display startup message
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(2);
  tft.setCursor(30, 140);
  tft.println(F("Initializing"));
  tft.setCursor(55, 165);
  tft.println(F("System..."));

  // Initialize RTC
  Serial.print(F("Initializing RTC..."));
  if (!rtc.begin()) {
    Serial.println(F("RTC not found!"));
    tft.setTextColor(COLOR_SPEED);
    tft.setTextSize(1);
    tft.setCursor(40, 200);
    tft.println(F("RTC Error!"));
    tft.setCursor(20, 215);
    tft.println(F("Check wiring"));
    while (1) delay(1000); // Halt
  }

  // Check if RTC lost power
  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power, setting time!"));
    // Set to compile time if you want:
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Or set manually - example: Jan 1, 2024, 12:00:00
    rtc.adjust(DateTime(2024, 1, 1, 12, 0, 0));
  }
  Serial.println(F("RTC OK"));

  // Initialize SD Card
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card failed or not present"));
    tft.setTextColor(COLOR_SPEED);
    tft.setTextSize(1);
    tft.setCursor(40, 235);
    tft.println(F("SD Card Error!"));
    tft.setCursor(20, 250);
    tft.println(F("Data won't save"));
    delay(3000);
  } else {
    Serial.println(F("SD Card initialized"));
    loadDataFromSD();
  }

  // Get current day for tracking
  DateTime now = rtc.now();
  lastDay = now.day();

  delay(2000);

  // Draw initial display
  drawStaticUI();
  updateAllDisplayValues();

  Serial.println(F("Setup complete!"));
}

// ==================== MAIN LOOP ====================
void loop() {
  // Check for day change (midnight reset)
  checkDayChange();

  // Read hall sensor
  bool currentHallState = digitalRead(HALL_SENSOR_PIN);

  // Detect magnet pass (HIGH to LOW transition)
  if (lastHallState == HIGH && currentHallState == LOW) {
    handleMagnetDetection();
  }

  lastHallState = currentHallState;

  // Check for speed timeout
  if (millis() - lastValidMagnetTime > SPEED_TIMEOUT && currentSpeed != 0.0) {
    currentSpeed = 0.0;
    updateSpeedDisplay();
  }

  // Check buttons
  handleButtons();

  // Small delay to prevent overwhelming the loop
  delay(10);
}

// ==================== MAGNET DETECTION ====================
void handleMagnetDetection() {
  unsigned long currentTime = millis();
  unsigned long timeSinceLastMagnet = currentTime - lastMagnetTime;

  // Debounce - ignore if too soon after last detection
  if (timeSinceLastMagnet < MIN_MAGNET_INTERVAL) {
    return; // Likely a bounce or false trigger
  }

  lastMagnetTime = currentTime;

  // Calculate speed (only if we have a previous valid reading)
  if (lastValidMagnetTime > 0) {
    unsigned long timeDiff = currentTime - lastValidMagnetTime;

    // Calculate speed in MPH
    // Speed = (distance / time) converted to MPH
    // distance in inches, time in milliseconds
    float calculatedSpeed = (DISTANCE_PER_MAGNET / timeDiff) * 3600000.0 / INCHES_PER_MILE;

    // Validate speed (reject if impossibly fast for a cat)
    if (calculatedSpeed <= MAX_CAT_SPEED) {
      currentSpeed = calculatedSpeed;
      lastValidMagnetTime = currentTime;

      // Check if it's lifetime top speed
      if (currentSpeed > lifetimeTopSpeed) {
        lifetimeTopSpeed = currentSpeed;
        updateTopSpeedDisplay();
        saveDataToSD();
      }

      updateSpeedDisplay();
    } else {
      // Speed too high - false reading, ignore it
      Serial.print(F("Rejected speed: "));
      Serial.print(calculatedSpeed);
      Serial.println(F(" MPH (too fast)"));
      return;
    }
  } else {
    // First valid reading
    lastValidMagnetTime = currentTime;
  }

  // Update distance
  magnetCounter++;
  if (magnetCounter >= MAGNET_COUNT) {
    // Completed one full rotation
    float rotationDistance = WHEEL_CIRCUMFERENCE / INCHES_PER_MILE;

    // Add to today's distance
    todayDistance += rotationDistance;

    // Add to day or night miles based on current time
    if (isDaytime()) {
      dayMiles += rotationDistance;
      updateDayMilesDisplay();
    } else {
      nightMiles += rotationDistance;
      updateNightMilesDisplay();
    }

    // Add to lifetime total
    lifetimeTotalMiles += rotationDistance;
    magnetCounter = 0;

    updateTodayDistanceDisplay();
    updateTotalMilesDisplay();

    // Save to SD periodically (every rotation)
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
      // Button pressed
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
      // Button pressed
      resetAllStats();
    }
  }
  lastAllResetState = allResetState;
}

// ==================== DAY CHANGE DETECTION ====================
void checkDayChange() {
  DateTime now = rtc.now();
  if (now.day() != lastDay) {
    // New day! Reset today's distance
    todayDistance = 0.0;
    lastDay = now.day();
    updateTodayDistanceDisplay();
    saveDataToSD();
    Serial.println(F("New day - today's miles reset"));
  }
}

// ==================== RESET FUNCTIONS ====================
void resetTodayStats() {
  todayDistance = 0.0;
  magnetCounter = 0;
  updateTodayDistanceDisplay();
  saveDataToSD();

  Serial.println(F("Today's miles reset"));
}

void resetAllStats() {
  lifetimeTopSpeed = 0.0;
  lifetimeTotalMiles = 0.0;
  dayMiles = 0.0;
  nightMiles = 0.0;
  todayDistance = 0.0;
  magnetCounter = 0;

  updateAllDisplayValues();
  saveDataToSD();

  Serial.println(F("All high scores reset"));
}

// ==================== DAY/NIGHT CALCULATION ====================
bool isDaytime() {
  DateTime now = rtc.now();

  // Calculate sunrise and sunset for today
  int dayOfYear = getDayOfYear(now.year(), now.month(), now.day());
  float sunriseHour = calculateSunrise(dayOfYear);
  float sunsetHour = calculateSunset(dayOfYear);

  // Convert current time to decimal hours
  float currentHour = now.hour() + now.minute() / 60.0;

  // Check if current time is between sunrise and sunset
  return (currentHour >= sunriseHour && currentHour < sunsetHour);
}

int getDayOfYear(int year, int month, int day) {
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Check for leap year
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
  // Simplified sunrise calculation for Chicago
  // This gives approximate sunrise time in decimal hours (Central Time)
  float amplitude = 2.5; // Hours of variation
  float offset = 6.3;    // Average sunrise time
  float phase = (dayOfYear - 172) * 2.0 * 3.14159 / 365.0; // Peak at summer solstice

  return offset - amplitude * cos(phase);
}

float calculateSunset(int dayOfYear) {
  // Simplified sunset calculation for Chicago
  // This gives approximate sunset time in decimal hours (Central Time)
  float amplitude = 2.5; // Hours of variation
  float offset = 18.5;   // Average sunset time
  float phase = (dayOfYear - 172) * 2.0 * 3.14159 / 365.0; // Peak at summer solstice

  return offset + amplitude * cos(phase);
}

// ==================== SD CARD FUNCTIONS ====================
void loadDataFromSD() {
  File dataFile = SD.open("catwheel.txt", FILE_READ);
  if (dataFile) {
    // Read data from file
    // Format: todayDistance,dayMiles,nightMiles,lifetimeTopSpeed,lifetimeTotalMiles
    String line = dataFile.readStringUntil('\n');
    dataFile.close();

    // Parse the data
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

      Serial.println(F("Data loaded from SD"));
    }
  } else {
    Serial.println(F("No existing data file, starting fresh"));
  }
}

void saveDataToSD() {
  // Delete old file
  if (SD.exists("catwheel.txt")) {
    SD.remove("catwheel.txt");
  }

  // Write new data
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

    Serial.println(F("Data saved to SD"));
  } else {
    Serial.println(F("Error writing to SD card"));
  }
}

// ==================== DISPLAY FUNCTIONS ====================
void drawStaticUI() {
  tft.fillScreen(COLOR_BG);

  // Current Speed Label
  tft.setTextColor(COLOR_LABEL);
  tft.setTextSize(2);
  tft.setCursor(10, 5);
  tft.println(F("Speed"));

  // Miles Today Label
  tft.setCursor(10, 85);
  tft.println(F("Miles Today"));

  // High Scores Section
  tft.drawLine(0, 140, 240, 140, COLOR_DAILY);
  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);
  tft.setCursor(35, 145);
  tft.println(F("HIGH SCORES"));
  tft.drawLine(0, 170, 240, 170, COLOR_DAILY);

  // High Score Labels (compact)
  tft.setTextColor(COLOR_LABEL);
  tft.setTextSize(1);

  tft.setCursor(5, 180);
  tft.println(F("Top Speed:"));

  tft.setCursor(5, 210);
  tft.println(F("Total Miles:"));

  tft.setCursor(5, 240);
  tft.println(F("Day Miles:"));

  tft.setCursor(5, 270);
  tft.println(F("Night Miles:"));

  // Button hints at bottom (smaller)
  tft.setTextColor(COLOR_LABEL);
  tft.setTextSize(1);
  tft.setCursor(3, 305);
  tft.println(F("Reset Day:B1 All:B2"));
}

void updateAllDisplayValues() {
  updateSpeedDisplay();
  updateTodayDistanceDisplay();
  updateTopSpeedDisplay();
  updateTotalMilesDisplay();
  updateDayMilesDisplay();
  updateNightMilesDisplay();
}

void updateSpeedDisplay() {
  // Clear previous value area
  tft.fillRect(0, 30, 240, 50, COLOR_BG);

  // Display current speed (large and prominent)
  tft.setTextColor(COLOR_SPEED);
  tft.setTextSize(5);

  char speedStr[10];
  dtostrf(currentSpeed, 4, 1, speedStr);

  tft.setCursor(15, 35);
  tft.print(speedStr);
  tft.setTextSize(2);
  tft.print(F(" MPH"));
}

void updateTodayDistanceDisplay() {
  // Clear previous value area
  tft.fillRect(0, 110, 240, 25, COLOR_BG);

  tft.setTextColor(COLOR_DISTANCE);
  tft.setTextSize(3);

  char distStr[10];
  dtostrf(todayDistance, 5, 1, distStr);

  tft.setCursor(45, 112);
  tft.print(distStr);
  tft.setTextSize(2);
  tft.print(F(" mi"));
}

void updateTopSpeedDisplay() {
  // Clear previous value area
  tft.fillRect(75, 177, 160, 18, COLOR_BG);

  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);

  char speedStr[10];
  dtostrf(lifetimeTopSpeed, 4, 1, speedStr);

  tft.setCursor(80, 178);
  tft.print(speedStr);
  tft.print(F(" MPH"));
}

void updateTotalMilesDisplay() {
  // Clear previous value area
  tft.fillRect(85, 207, 150, 18, COLOR_BG);

  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);

  char distStr[10];
  dtostrf(lifetimeTotalMiles, 6, 1, distStr);

  tft.setCursor(90, 208);
  tft.print(distStr);
  tft.print(F(" mi"));
}

void updateDayMilesDisplay() {
  // Clear previous value area
  tft.fillRect(75, 237, 160, 18, COLOR_BG);

  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);

  char distStr[10];
  dtostrf(dayMiles, 6, 1, distStr);

  tft.setCursor(80, 238);
  tft.print(distStr);
  tft.print(F(" mi"));
}

void updateNightMilesDisplay() {
  // Clear previous value area
  tft.fillRect(85, 267, 150, 18, COLOR_BG);

  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);

  char distStr[10];
  dtostrf(nightMiles, 6, 1, distStr);

  tft.setCursor(90, 268);
  tft.print(distStr);
  tft.print(F(" mi"));
}
