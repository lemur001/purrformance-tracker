/*
 * Simple Display Test for Mega 2560
 * Tests ONLY the display - no SD card, no RTC, nothing else
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Pin Definitions for Mega 2560
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8
#define TFT_LED   3
#define BRIGHTNESS_POT A0

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Set up brightness control
  pinMode(TFT_LED, OUTPUT);
  pinMode(BRIGHTNESS_POT, INPUT);

  // Set brightness to mid-level
  int brightness = map(analogRead(BRIGHTNESS_POT), 0, 1023, 20, 255);
  analogWrite(TFT_LED, brightness);

  // Small delay
  delay(100);

  // Initialize display
  tft.begin();

  // Small delay after init
  delay(100);

  // Fill screen red
  tft.fillScreen(ILI9341_RED);
  delay(500);

  // Fill screen green
  tft.fillScreen(ILI9341_GREEN);
  delay(500);

  // Fill screen blue
  tft.fillScreen(ILI9341_BLUE);
  delay(500);

  // Black background
  tft.fillScreen(ILI9341_BLACK);

  // Draw white text
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(50, 100);
  tft.println("DISPLAY");
  tft.setCursor(70, 130);
  tft.println("WORKS!");

  // Draw colored rectangles
  tft.fillRect(20, 200, 50, 50, ILI9341_RED);
  tft.fillRect(85, 200, 50, 50, ILI9341_GREEN);
  tft.fillRect(150, 200, 50, 50, ILI9341_BLUE);
}

void loop() {
  // Update brightness continuously
  int brightness = map(analogRead(BRIGHTNESS_POT), 0, 1023, 20, 255);
  analogWrite(TFT_LED, brightness);
  delay(50);
}
