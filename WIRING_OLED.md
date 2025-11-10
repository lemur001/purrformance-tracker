# Cat Wheel Speedometer - OLED Wiring Guide (Mega 2560)

## Pin Connections Summary

```
MEGA 2560 PIN CONNECTIONS (OLED VERSION)
=========================================

Power:
  5V  ──► OLED VCC, RTC VCC, Hall Sensor VCC, SD VCC
  GND ──► OLED GND, RTC GND, Hall Sensor GND, SD GND, Button 1, Button 2

I2C Bus (shared by OLED and RTC):
  D20 (SDA) ──► OLED SDA, RTC SDA
  D21 (SCL) ──► OLED SCL, RTC SCL

SPI Bus (SD Card only):
  D51 (MOSI) ──► SD MOSI
  D52 (SCK)  ──► SD SCK
  D50 (MISO) ──► SD MISO
  D4  (CS)   ──► SD CS

Hall Sensor:
  D5  ──► Hall Sensor Signal (Y - yellow wire)

Buttons:
  D6  ──► Today Reset Button (other leg to GND)
  D7  ──► All Reset Button (other leg to GND)
```

## Component Wiring

### OLED Display (SSD1306 - I2C)

```
OLED Pin    →    Mega 2560 Pin
--------------------------------
VCC         →    5V (or 3.3V - check your OLED specs)
GND         →    GND
SDA         →    D20 (I2C data)
SCL         →    D21 (I2C clock)
```

**Notes:**
- OLED shares I2C bus with RTC (this is normal and correct)
- Most SSD1306 OLEDs work with 5V, but some require 3.3V - check your display
- I2C address is usually 0x3C (if display doesn't work, try changing to 0x3D in code)

### RTC Module (DS3231 - I2C)

```
RTC Pin     →    Mega 2560 Pin
--------------------------------
VCC         →    5V
GND         →    GND
SDA         →    D20 (shared with OLED)
SCL         →    D21 (shared with OLED)
```

**Notes:**
- RTC and OLED share the same I2C bus (D20/D21)
- Make sure CR2032 battery is installed in RTC module

### SD Card (on separate module or TFT backpack)

```
SD Pin      →    Mega 2560 Pin
--------------------------------
CS          →    D4
MOSI        →    D51
SCK         →    D52
MISO        →    D50
VCC         →    5V
GND         →    GND
```

### Hall Sensor (G R Y labeling)

```
Hall Pin         →    Mega 2560 Pin
--------------------------------
G (Green - GND)  →    GND
R (Red - VCC)    →    5V
Y (Yellow - Sig) →    D5
```

### Buttons

```
Component           →    Mega 2560 Pin
--------------------------------
Today Reset Button  →    D6 (other leg to GND)
All Reset Button    →    D7 (other leg to GND)
```

**For 4-pin tactile buttons:** Use diagonal pins (opposite corners)

## Breadboard Layout

```
Mega 2560          OLED Display
  D20 (SDA) ──────────► SDA
  D21 (SCL) ──────────► SCL
     5V    ──────────► VCC
    GND    ──────────► GND

Mega 2560          RTC Module (shares I2C with OLED)
  D20 (SDA) ──────────► SDA
  D21 (SCL) ──────────► SCL
     5V    ──────────► VCC
    GND    ──────────► GND

Mega 2560          SD Card Module
    D4     ──────────► CS
   D51     ──────────► MOSI
   D52     ──────────► SCK
   D50     ◄──────────┤ MISO
    5V     ──────────► VCC
   GND     ──────────► GND

Mega 2560          Hall Sensor
    D5     ◄──────────┤ Y (Yellow)
    5V     ──────────► R (Red)
   GND     ──────────► G (Green)

Mega 2560          Buttons
    D6     ──────────► Button 1 ──────► GND
    D7     ──────────► Button 2 ──────► GND
```

## Upload Instructions

1. Open **CatWheelSpeedometer_OLED.ino** in Arduino IDE
2. Install required libraries:
   - Adafruit GFX Library
   - Adafruit SSD1306
   - RTClib
3. Select **Tools → Board → Arduino Mega 2560**
4. Select your COM port
5. Upload

## Display Layout (8 Lines)

```
Speed:  12.3MPH     ← Current speed (large numbers)
Today:   5.2 mi     ← Miles today
---HIGH SCORES---   ← Separator
Top:    15.8 MPH    ← All-time top speed
Total: 234.5 mi     ← Lifetime total miles
Day:   156.3 mi     ← Total day miles
Night:  78.2 mi     ← Total night miles
                    ← (Button info removed to save space)
```

## Troubleshooting

**Display shows nothing:**
- Check I2C address - try changing `SCREEN_ADDRESS` to `0x3D` if `0x3C` doesn't work
- Check VCC voltage - some OLEDs need 3.3V instead of 5V
- Verify SDA/SCL are on D20/D21

**Display works but RTC doesn't:**
- Both should share the same I2C bus
- Check RTC has CR2032 battery installed
- Check all I2C connections

**SD card fails:**
- Verify CS pin is D4
- Check SPI pins: 51, 52, 50
- Format SD card as FAT32

## Quick Checklist

- [ ] OLED VCC to 5V (or 3.3V if required)
- [ ] OLED GND to GND
- [ ] OLED SDA to D20
- [ ] OLED SCL to D21
- [ ] RTC shares same SDA/SCL (D20/D21)
- [ ] SD CS to D4
- [ ] SD MOSI/SCK/MISO to D51/D52/D50
- [ ] Hall sensor Y to D5
- [ ] Buttons to D6 and D7
- [ ] All grounds connected together
