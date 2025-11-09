/*
 * Cat Wheel Speedometer & Odometer
 *
 * Hardware:
 * - 3.2" TFT SPI 240x320 v1.0 Display with SD Card
 * - Hall Effect Sensor
 * - 12 Magnets on cat wheel
 * - Arduino (Uno/Mega recommended for memory)
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
#define DAILY_RESET_BTN    6    // Button to reset daily stats
#define TRIP_RESET_BTN     7    // Button to reset trip odometer

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

// Speed and Distance Tracking
unsigned long lastMagnetTime = 0;
unsigned long lastValidMagnetTime = 0;
float currentSpeed = 0.0;           // Current speed in MPH
float tripOdometer = 0.0;           // Current trip in miles
int magnetCounter = 0;              // Count magnets for distance calculation

// Daily Leaderboard
float dailyTopSpeed = 0.0;          // Highest speed today
float dailyDistance = 0.0;          // Total distance today

// Lifetime Stats (stored on SD)
float lifetimeTopSpeed = 0.0;       // All-time top speed
float lifetimeTotalMiles = 0.0;     // All-time total miles

// Hall Sensor State
bool lastHallState = HIGH;

// Button States
bool lastDailyResetState = HIGH;
bool lastTripResetState = HIGH;
unsigned long lastDailyResetDebounce = 0;
unsigned long lastTripResetDebounce = 0;

// Display Update Flags
bool needsFullRedraw = true;

// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);
  Serial.println(F("Cat Wheel Speedometer Starting..."));

  // Initialize pins
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  pinMode(DAILY_RESET_BTN, INPUT_PULLUP);
  pinMode(TRIP_RESET_BTN, INPUT_PULLUP);

  // Initialize TFT Display
  tft.begin();
  tft.setRotation(0);  // Portrait mode (240 wide x 320 tall)
  tft.fillScreen(COLOR_BG);

  // Display startup message
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(2);
  tft.setCursor(30, 150);
  tft.println(F("Cat Wheel"));
  tft.setCursor(15, 175);
  tft.println(F("Speedometer"));

  // Initialize SD Card
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card failed or not present"));
    tft.setTextColor(COLOR_SPEED);
    tft.setTextSize(1);
    tft.setCursor(40, 210);
    tft.println(F("SD Card Error!"));
    tft.setCursor(20, 225);
    tft.println(F("Data won't be saved"));
    delay(3000);
  } else {
    Serial.println(F("SD Card initialized"));
    loadDataFromSD();
  }

  delay(2000);

  // Draw initial display
  drawStaticUI();
  updateAllDisplayValues();

  Serial.println(F("Setup complete!"));
}

// ==================== MAIN LOOP ====================
void loop() {
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

      // Update daily top speed
      if (currentSpeed > dailyTopSpeed) {
        dailyTopSpeed = currentSpeed;
        updateDailyTopSpeedDisplay();

        // Check if it's also lifetime top speed
        if (dailyTopSpeed > lifetimeTopSpeed) {
          lifetimeTopSpeed = dailyTopSpeed;
          saveDataToSD();
        }
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
    tripOdometer += rotationDistance;
    dailyDistance += rotationDistance;
    lifetimeTotalMiles += rotationDistance;
    magnetCounter = 0;

    updateTripOdometerDisplay();
    updateDailyDistanceDisplay();

    // Save to SD periodically (every rotation)
    saveDataToSD();
  }
}

// ==================== BUTTON HANDLING ====================
void handleButtons() {
  unsigned long currentTime = millis();

  // Daily Reset Button
  bool dailyResetState = digitalRead(DAILY_RESET_BTN);
  if (dailyResetState != lastDailyResetState) {
    lastDailyResetDebounce = currentTime;
  }
  if ((currentTime - lastDailyResetDebounce) > DEBOUNCE_DELAY) {
    if (dailyResetState == LOW && lastDailyResetState == HIGH) {
      // Button pressed
      resetDailyStats();
    }
  }
  lastDailyResetState = dailyResetState;

  // Trip Reset Button
  bool tripResetState = digitalRead(TRIP_RESET_BTN);
  if (tripResetState != lastTripResetState) {
    lastTripResetDebounce = currentTime;
  }
  if ((currentTime - lastTripResetDebounce) > DEBOUNCE_DELAY) {
    if (tripResetState == LOW && lastTripResetState == HIGH) {
      // Button pressed
      resetTripOdometer();
    }
  }
  lastTripResetState = tripResetState;
}

// ==================== RESET FUNCTIONS ====================
void resetDailyStats() {
  dailyTopSpeed = 0.0;
  dailyDistance = 0.0;
  updateDailyTopSpeedDisplay();
  updateDailyDistanceDisplay();
  saveDataToSD();

  Serial.println(F("Daily stats reset"));
}

void resetTripOdometer() {
  tripOdometer = 0.0;
  magnetCounter = 0;
  updateTripOdometerDisplay();

  Serial.println(F("Trip odometer reset"));
}

// ==================== SD CARD FUNCTIONS ====================
void loadDataFromSD() {
  File dataFile = SD.open("catwheel.txt", FILE_READ);
  if (dataFile) {
    // Read data from file
    // Format: dailyTopSpeed,dailyDistance,lifetimeTopSpeed,lifetimeTotalMiles
    String line = dataFile.readStringUntil('\n');
    dataFile.close();

    // Parse the data
    int firstComma = line.indexOf(',');
    int secondComma = line.indexOf(',', firstComma + 1);
    int thirdComma = line.indexOf(',', secondComma + 1);

    if (firstComma > 0 && secondComma > 0 && thirdComma > 0) {
      dailyTopSpeed = line.substring(0, firstComma).toFloat();
      dailyDistance = line.substring(firstComma + 1, secondComma).toFloat();
      lifetimeTopSpeed = line.substring(secondComma + 1, thirdComma).toFloat();
      lifetimeTotalMiles = line.substring(thirdComma + 1).toFloat();

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
    dataFile.print(dailyTopSpeed, 1);
    dataFile.print(',');
    dataFile.print(dailyDistance, 1);
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

  // Title
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(3);
  tft.setCursor(15, 10);
  tft.println(F("CAT WHEEL"));

  // Divider line
  tft.drawLine(0, 45, 240, 45, COLOR_TITLE);

  // Current Speed Label
  tft.setTextColor(COLOR_LABEL);
  tft.setTextSize(2);
  tft.setCursor(10, 55);
  tft.println(F("Current Speed"));

  // Trip Odometer Label
  tft.setCursor(10, 130);
  tft.println(F("Trip Miles"));

  // Divider line
  tft.drawLine(0, 180, 240, 180, COLOR_DAILY);

  // Daily Leaderboard Title
  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);
  tft.setCursor(25, 190);
  tft.println(F("DAILY BEST"));

  // Daily Top Speed Label
  tft.setTextColor(COLOR_LABEL);
  tft.setTextSize(1);
  tft.setCursor(10, 220);
  tft.println(F("Top Speed:"));

  // Daily Distance Label
  tft.setCursor(10, 260);
  tft.println(F("Total Miles:"));

  // Instructions at bottom
  tft.setTextColor(COLOR_LABEL);
  tft.setTextSize(1);
  tft.setCursor(5, 305);
  tft.println(F("D6:Daily D7:Trip Reset"));
}

void updateAllDisplayValues() {
  updateSpeedDisplay();
  updateTripOdometerDisplay();
  updateDailyTopSpeedDisplay();
  updateDailyDistanceDisplay();
}

void updateSpeedDisplay() {
  // Clear previous value area
  tft.fillRect(0, 80, 240, 40, COLOR_BG);

  // Display current speed (large and prominent)
  tft.setTextColor(COLOR_SPEED);
  tft.setTextSize(4);

  char speedStr[10];
  dtostrf(currentSpeed, 5, 1, speedStr);

  // Center the text
  int16_t x = 30;
  tft.setCursor(x, 85);
  tft.print(speedStr);
  tft.setTextSize(2);
  tft.print(F(" MPH"));
}

void updateTripOdometerDisplay() {
  // Clear previous value area
  tft.fillRect(0, 150, 240, 25, COLOR_BG);

  tft.setTextColor(COLOR_DISTANCE);
  tft.setTextSize(3);

  char distStr[10];
  dtostrf(tripOdometer, 6, 1, distStr);

  tft.setCursor(35, 152);
  tft.print(distStr);
  tft.setTextSize(2);
  tft.print(F(" mi"));
}

void updateDailyTopSpeedDisplay() {
  // Clear previous value area
  tft.fillRect(100, 215, 135, 20, COLOR_BG);

  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);

  char speedStr[10];
  dtostrf(dailyTopSpeed, 5, 1, speedStr);

  tft.setCursor(105, 217);
  tft.print(speedStr);
  tft.print(F(" MPH"));
}

void updateDailyDistanceDisplay() {
  // Clear previous value area
  tft.fillRect(100, 255, 135, 20, COLOR_BG);

  tft.setTextColor(COLOR_DAILY);
  tft.setTextSize(2);

  char distStr[10];
  dtostrf(dailyDistance, 6, 1, distStr);

  tft.setCursor(105, 257);
  tft.print(distStr);
  tft.print(F(" mi"));
}
