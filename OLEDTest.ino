/*
 * Simple OLED Test - Tests display only
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Try 0x3C first, if nothing shows try 0x3D
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  // Try to initialize display
  Serial.println("Initializing OLED...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("FAILED! Display not found.");
    Serial.println("Try:");
    Serial.println("1. Change address to 0x3D in line 13");
    Serial.println("2. Check VCC (try 3.3V instead of 5V)");
    Serial.println("3. Verify SDA to D20, SCL to D21");
    while(1);
  }

  Serial.println("SUCCESS! Display initialized.");

  // Clear and show test pattern
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("DISPLAY");
  display.setCursor(25, 40);
  display.println("WORKS!");
  display.display();

  Serial.println("Test pattern displayed.");
}

void loop() {
  // Blink display every second
  delay(1000);
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
}
