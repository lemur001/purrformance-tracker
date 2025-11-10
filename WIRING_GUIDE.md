# Cat Wheel Speedometer - Wiring Guide for Arduino Mega 2560

## Overview
This guide provides step-by-step wiring instructions for connecting all components to your Arduino Mega 2560.

## Safety First
- ⚡ Disconnect power before making connections
- 🔌 Double-check all connections before powering on
- 🔍 Never connect 3.3V components to 5V pins (this display is 5V tolerant)
- 🔥 Avoid short circuits - keep VCC and GND separate

## Pin Assignment Summary (Arduino Mega 2560)

| Component | Pin Type | Mega 2560 Pin | Notes |
|-----------|----------|---------------|-------|
| TFT CS | Digital Output | D10 | Chip Select for display |
| TFT DC | Digital Output | D9 | Data/Command select |
| TFT RST | Digital Output | D8 | Reset |
| TFT LED | PWM Output | D3 | Backlight control |
| TFT MOSI | SPI | D51 | Hardware SPI (can't change) |
| TFT SCK | SPI | D52 | Hardware SPI (can't change) |
| TFT MISO | SPI | D50 | Hardware SPI (can't change) |
| SD CS | Digital Output | D4 | SD Card Chip Select |
| RTC SDA | I2C | D20 | I2C Data (can't change) |
| RTC SCL | I2C | D21 | I2C Clock (can't change) |
| Hall Sensor | Digital Input | D5 | With internal pullup |
| Today Reset | Digital Input | D6 | Button with internal pullup |
| All Reset | Digital Input | D7 | Button with internal pullup |
| Brightness Pot | Analog Input | A0 | 10k potentiometer |

## Step-by-Step Wiring

### Step 1: TFT Display (Primary Component)

The 3.2" TFT display typically has pins along one edge. Pin labels may be on the back.

```
TFT Display Pin Connections (Arduino Mega 2560):
================================================

TFT Pin    Wire Color*   Mega 2560 Pin   Notes
--------   -----------   -------------   -----
VCC        Red           5V              Power (5V rail)
GND        Black         GND             Ground
CS         Orange        D10             Chip Select
RESET      Yellow        D8              Reset pin
DC/RS      Green         D9              Data/Command
MOSI/SDI   Blue          D51             Master Out Slave In (SPI)
SCK/CLK    Purple        D52             Serial Clock (SPI)
LED        (via pot)     D3              Backlight - see brightness section
MISO/SDO   Gray          D50             Master In Slave Out (SPI)

*Wire colors are suggestions for easy identification
```

**Important Notes:**
- Some displays have additional pins (3.3V, NC) - leave these unconnected
- LED pin: Connected through brightness control circuit (see Step 7)
- If display has a touch function (T_* pins), you can leave these unconnected
- CRITICAL: Mega 2560 uses different SPI pins than Uno/Nano (51/52/50 not 11/13/12)

**Breadboard Layout for TFT:**
```
Arduino Mega 2560          TFT Display
                          ┌─────────┐
    5V  ────────────────► │ VCC     │
   GND  ────────────────► │ GND     │
   D10  ────────────────► │ CS      │
    D8  ────────────────► │ RESET   │
    D9  ────────────────► │ DC      │
   D51  ────────────────► │ MOSI    │
   D52  ────────────────► │ SCK     │
    D3  ────────────────► │ LED     │ (via brightness circuit)
   D50  ◄────────────────┤ MISO    │
                          └─────────┘
```

### Step 2: RTC Module (I2C)

The DS3231 or DS1307 RTC module keeps track of time, even when power is disconnected (thanks to backup battery).

```
RTC Module Pin Connections (Arduino Mega 2560):
==============================================

RTC Pin    Wire Color*   Mega 2560 Pin   Notes
--------   -----------   -------------   -----
VCC        Red           5V              Power
GND        Black         GND             Ground
SDA        Blue          D20             I2C Data
SCL        Yellow        D21             I2C Clock

*Wire colors are suggestions for easy identification
```

**Important Notes:**
- RTC uses I2C communication (2-wire)
- On Mega 2560, SDA = D20, SCL = D21 (hardware-specific, cannot be changed)
- Most RTC modules include pull-up resistors (no external resistors needed)
- CR2032 battery should be installed on RTC module for time backup
- Some modules have an additional SQW (Square Wave) pin - leave it unconnected

**Battery Installation:**
```
DS3231/DS1307 Module
┌─────────────────┐
│  CR2032 Slot    │  ← Insert CR2032 battery here
│  (back of PCB)  │     Positive (+) side usually up
└─────────────────┘
```

### Step 3: SD Card (on TFT Module)

Most 3.2" TFT displays have an SD card slot on the back. These share the SPI bus with the display.

```
SD Card Pin (on TFT back)   Mega 2560 Pin   Notes
-------------------------   -------------   -----
SD_CS                       D4              Separate chip select
SD_MOSI                     D51             Shared with TFT
SD_SCK                      D52             Shared with TFT
SD_MISO                     D50             Shared with TFT
(VCC)                       5V              Usually shared with TFT VCC
(GND)                       GND             Usually shared with TFT GND
```

**Important:**
- SD card shares MOSI, MISO, SCK with TFT
- SD card has its own CS pin - this must go to D4
- Some displays have SD pins pre-connected internally; you may only need to connect SD_CS to D4
- Check your display's documentation

**If SD pins are separate (not common):**
```
Mega 2560    SD Card Slot
  D4  ──────► CS
  D51 ──────► MOSI
  D52 ──────► SCK
  D50 ◄────── MISO
  5V  ──────► VCC
  GND ──────► GND
```

### Step 4: Hall Effect Sensor

The Hall sensor detects magnets passing by. Typically in a TO-92 package (3 pins).

```
Hall Sensor (A3144 common pinout)

   Front View (flat side facing you):
        _____
       |     |
       |_____|
        | | |
        1 2 3

   Pin 1: VCC  (leftmost when flat side faces you)
   Pin 2: GND  (middle)
   Pin 3: OUT  (rightmost)

⚠️ CRITICAL: Pinout varies by sensor model! Always verify with datasheet!
```

**Wiring:**
```
Hall Sensor Pin    Wire Color*    Arduino Pin
---------------    -----------    -----------
Pin 1 (VCC)        Red            5V
Pin 2 (GND)        Black          GND
Pin 3 (OUT)        Yellow         D5

*Suggested colors
```

**Physical Mounting:**
1. Position sensor 1-10mm from magnet path on wheel rim
2. Sensor should not touch moving parts
3. Mount rigidly to prevent vibration
4. Test by manually rotating wheel and checking Serial Monitor

**Wiring Options:**

**Option A: Direct Connection (Recommended)**
```
Arduino                Hall Sensor
  5V  ──────────────────► Pin 1 (VCC)
  GND ──────────────────► Pin 2 (GND)
  D5  ◄──────────────────┤ Pin 3 (OUT)

Note: D5 has internal pullup enabled in code
```

**Option B: With External Pullup Resistor (if sensor is open-collector)**
```
                    ┌─── 5V
                    │
                   [R]  10kΩ resistor
                    │
  D5 ◄──────────────┼─── Hall Sensor OUT (Pin 3)

  GND ◄────────────────── Hall Sensor GND (Pin 2)
  5V  ──────────────────► Hall Sensor VCC (Pin 1)
```

### Step 5: Push Buttons

Two momentary push buttons for reset functions.

**Button Wiring (per button):**
```
Button 1 (Today Reset):

   Arduino D6 ──────┐
                    │
                 ┌──┴──┐
                 │ BTN │  Momentary Push Button
                 └──┬──┘
                    │
   GND  ────────────┘

Internal pullup resistor enabled in code (no external resistor needed)
```

```
Button 2 (All Reset):

   Arduino D7 ──────┐
                    │
                 ┌──┴──┐
                 │ BTN │  Momentary Push Button
                 └──┬──┘
                    │
   GND  ────────────┘

Internal pullup resistor enabled in code (no external resistor needed)
```

**Breadboard Layout:**
```
For each button:
  - One leg to Arduino pin (D6 or D7)
  - Other leg to GND rail
  - Button pressed = connects pin to GND (reads LOW)
  - Button released = pin pulled HIGH by internal resistor
```

**4-Pin Tactile Switch:**
Many tactile switches have 4 pins (2 pairs). Pins are connected in pairs:
```
    1 ──── 2         Use pins 1 and 3 (or 2 and 4)
    │      │         Don't use pins on same side
    3 ──── 4
```

### Step 6: Brightness Control (10k Potentiometer)

The 10k potentiometer allows you to adjust the LCD backlight brightness.

```
10k Potentiometer Connections:
==============================

Pot Pin      Mega 2560 Pin   Notes
--------     -------------   -----
Pin 1        GND             Ground (one outer pin)
Pin 2        A0              Wiper/middle pin (analog input)
Pin 3        5V              Power (other outer pin)

Note: Pins 1 and 3 are interchangeable (swapping reverses rotation direction)
```

**LED (Backlight) Connection:**
```
Display LED Pin ──► Mega D3 (PWM)
```

**How It Works:**
- Potentiometer creates variable voltage on A0 (0V to 5V)
- Arduino reads voltage and converts to brightness value (0-255)
- PWM on D3 controls LED brightness
- Minimum brightness set to 20/255 so display never goes completely dark

**Breadboard Layout:**
```
Mega 2560         Potentiometer        Display
                  ┌─────────┐
  5V  ────────────┤ 3  2  1 ├──────────► GND
                  │    │    │
  A0  ◄───────────┤    │    │
                  └────┴────┘

  D3  ─────────────────────────────────► LED pin
```

### Step 7: Power Supply

**Option A: USB Power (Easiest for Testing)**
```
Computer USB ──► Arduino USB Port ──► Powers everything

Pros: Simple, good for testing
Cons: Tethered to computer
```

**Option B: USB Wall Adapter**
```
USB Wall Adapter (5V 2A) ──► Arduino USB Port

Pros: Standalone operation
Cons: Needs nearby outlet
Recommended: Use quality adapter (phone chargers work well)
```

**Option C: DC Barrel Jack (7-12V)**
```
7-12V DC Adapter ──► Arduino DC Barrel Jack ──► Internal regulator ──► 5V

Pros: Higher voltage input
Cons: Wastes power as heat, less efficient
Note: Arduino regulates to 5V internally
```

**Option D: Battery Pack (Portable)**
```
9V Battery ──► 9V Battery Connector ──► Arduino DC Jack
    OR
6xAA (9V) Battery Holder ──► DC Jack

Pros: Completely portable
Cons: Batteries need replacement/recharging
Runtime: ~8-12 hours with 9V, longer with AA
```

### Step 8: Power Distribution on Breadboard

**Breadboard Power Rails:**
```
Arduino 5V Pin ──────┬──► Breadboard + (Red) Rail
                     │
                     ├──► TFT VCC
                     ├──► TFT LED
                     ├──► RTC VCC
                     └──► Hall Sensor VCC

Arduino GND Pin ─────┬──► Breadboard - (Blue/Black) Rail
                     │
                     ├──► TFT GND
                     ├──► RTC GND
                     ├──► Hall Sensor GND
                     ├──► Button 1 (other side)
                     └──► Button 2 (other side)
```

## Complete Wiring Summary (Arduino Mega 2560)

```
MEGA 2560 PIN CONNECTIONS
=========================

Power:
  5V  ──► TFT VCC, RTC VCC, Hall Sensor VCC, Potentiometer Pin 3
  GND ──► TFT GND, RTC GND, Hall Sensor GND, SD GND, Button 1, Button 2, Pot Pin 1

SPI Bus (shared by TFT and SD Card):
  D51 (MOSI) ──► TFT MOSI/SDI, SD MOSI
  D52 (SCK)  ──► TFT SCK/CLK, SD SCK
  D50 (MISO) ──► TFT MISO/SDO, SD MISO

TFT Display:
  D10 ──► TFT CS (chip select)
  D9  ──► TFT DC (data/command)
  D8  ──► TFT RESET
  D3  ──► TFT LED (backlight, PWM)

SD Card:
  D4  ──► SD CS (chip select)

RTC Module (I2C):
  D20 ──► RTC SDA (I2C data)
  D21 ──► RTC SCL (I2C clock)

Hall Sensor:
  D5  ──► Hall Sensor OUT (signal pin)

Buttons:
  D6  ──► Today Reset Button (other leg to GND)
  D7  ──► All Reset Button (other leg to GND)

Brightness Control:
  A0  ──► Potentiometer wiper (middle pin)
```

## Verification Checklist (MEGA 2560 SPECIFIC)

Before powering on, verify:

- [ ] SPI pins correct for Mega: MOSI=51, SCK=52, MISO=50 (NOT 11/13/12!)
- [ ] I2C pins correct for Mega: SDA=20, SCL=21 (NOT A4/A5!)
- [ ] TFT_CS (D10) and SD_CS (D4) are different pins
- [ ] TFT LED connects to D3 (PWM pin)
- [ ] Potentiometer wiper (middle pin) to A0
- [ ] Hall sensor VCC goes to 5V, not GND (polarity correct)
- [ ] All grounds connected together (common ground)
- [ ] No shorts between VCC and GND
- [ ] Buttons connect pin to GND (not VCC)
- [ ] SD card inserted in slot and formatted FAT32
- [ ] Magnets secured to wheel, same pole facing sensor

## Testing Each Component

### Test 1: TFT Display
Upload this simple test:
```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.begin();
  tft.fillScreen(0x001F); // Blue screen
  tft.setTextColor(0xFFFF);
  tft.setTextSize(3);
  tft.setCursor(50, 150);
  tft.println("HELLO!");
}

void loop() {}
```
**Expected**: Blue screen with "HELLO!" in white

### Test 2: SD Card
Open Serial Monitor (9600 baud) after uploading main sketch.
**Expected**: "SD Card initialized" message

### Test 3: Hall Sensor
Add to main sketch in `loop()`:
```cpp
Serial.print("Hall: ");
Serial.println(digitalRead(HALL_SENSOR_PIN));
delay(100);
```
**Expected**:
- Shows "1" (HIGH) normally
- Shows "0" (LOW) when magnet near sensor

### Test 4: Buttons
Press each button while watching Serial Monitor.
**Expected**: See debug messages for resets

## Common Issues

| Symptom | Likely Cause | Solution |
|---------|-------------|----------|
| White/blank display | Wrong wiring or pins | Verify all TFT connections |
| Display garbled | SPI speed too high | Try different Arduino board |
| SD card fail | Wrong CS pin | Verify SD_CS = D4 |
| No magnet detection | Sensor too far | Move sensor closer (1-5mm) |
| False speed readings | Magnets uneven | Reposition magnets evenly |
| Buttons don't work | Wired to VCC instead of GND | Swap button connection |

## Next Steps

Once wiring is complete:
1. Upload the main sketch (CatWheelSpeedometer.ino)
2. Open Serial Monitor to see debug messages
3. Manually spin wheel slowly to test detection
4. Mount on cat wheel and test with your cat!

Refer to README.md for full usage instructions.
