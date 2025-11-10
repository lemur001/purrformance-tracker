# Cat Wheel Speedometer - OLED Wiring Guide (Simplified)

## Components Required

- Arduino Mega 2560
- SSD1306 OLED Display (128x64, I2C)
- DS3231 RTC Module with CR2032 battery
- Hall Effect Sensor (G R Y pins)
- 1 Push Button (hold 3 seconds to reset all stats)
- Breadboard and jumper wires

## Pin Connections (Arduino Mega 2560)

```
OLED Display (SSD1306) → Mega 2560
-------------------------------------
VCC → 5V (or 3.3V - check your OLED)
GND → GND
SDA → D20
SCL → D21

RTC Module (DS3231) → Mega 2560
-------------------------------------
VCC → 5V
GND → GND
SDA → D20 (shares with OLED)
SCL → D21 (shares with OLED)

Hall Sensor (G R Y) → Mega 2560
-------------------------------------
G (Green - GND) → GND
R (Red - VCC)   → 5V
Y (Yellow - Sig) → D5

Reset Button → Mega 2560
-------------------------------------
One side → D6
Other side → GND
(Hold 3 seconds to reset all stats)
```

## Simple Breadboard Layout

```
Mega 2560          Components
=========          ==========

Power Rails:
  5V  ───────┬───► OLED VCC
             ├───► RTC VCC
             └───► Hall Sensor (R - Red)

  GND ───────┬───► OLED GND
             ├───► RTC GND
             ├───► Hall Sensor (G - Green)
             └───► Button (one side)

I2C Bus (shared):
  D20 (SDA) ─┬───► OLED SDA
             └───► RTC SDA

  D21 (SCL) ─┬───► OLED SCL
             └───► RTC SCL

Sensor:
  D5  ◄──────────── Hall Sensor (Y - Yellow)

Button:
  D6  ────────────► Reset Button
```

## Display Layout

```
----------DAILY----------
Speed:          12.3 MPH
Distance:         5.2 mi
-----HIGH SCORES-----
Top Speed:       15.8 MPH
Total Dist:     234.5 mi
Daytime:        156.3 mi
Nighttime:       78.2 mi
```

**Features:**
- Numbers are right-aligned and fixed-width
- Values won't shift position as they grow (99→100)
- Hold reset button for 3 seconds to clear all stats
- "RESET!" message displays for 1 second after reset

## Upload Instructions

1. **Install libraries** in Arduino IDE:
   - Adafruit GFX Library
   - Adafruit SSD1306
   - RTClib

2. **Open** CatWheelSpeedometer_OLED.ino

3. **Select** Tools → Board → Arduino Mega 2560

4. **Select** your COM port

5. **Upload** to the Mega 2560

## Troubleshooting

**Display shows nothing:**
- Try changing line 29 from `0x3C` to `0x3D`
- Some OLEDs need 3.3V instead of 5V (check yours)
- Verify SDA/SCL on D20/D21 (NOT A4/A5!)

**RTC doesn't work:**
- Check CR2032 battery is installed in RTC module
- RTC shares same I2C bus (D20/D21) with OLED

**Hall sensor not detecting:**
- Verify Yellow wire to D5
- Check sensor is 1-5mm from magnet path
- Try moving sensor closer

**Reset button doesn't work:**
- For 4-pin buttons, use diagonal pins (opposite corners)
- Must hold for full 3 seconds
- Look for "RESET!" message on display

## Quick Checklist

- [ ] OLED VCC to 5V (or 3.3V)
- [ ] OLED GND to GND
- [ ] OLED SDA to D20
- [ ] OLED SCL to D21
- [ ] RTC SDA to D20 (same as OLED)
- [ ] RTC SCL to D21 (same as OLED)
- [ ] RTC has CR2032 battery installed
- [ ] Hall sensor Y (Yellow) to D5
- [ ] Hall sensor R (Red) to 5V
- [ ] Hall sensor G (Green) to GND
- [ ] Button to D6 and GND
- [ ] All grounds connected together

## Notes

- **No SD card** - Stats stored in RAM only (lost on power cycle)
- **No brightness control** - OLED is self-illuminating
- **Single button** - Hold 3 seconds to reset all stats
- **RTC battery required** - For day/night tracking and midnight auto-reset
- **Today's distance** - Auto-resets at midnight
- **Day/Night miles** - Calculated based on Chicago sunrise/sunset times
