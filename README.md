# Cat Wheel Speedometer & Odometer

A comprehensive Arduino-based speedometer and odometer system for cat exercise wheels, featuring real-time speed tracking, trip odometer, daily leaderboards, and persistent data storage.

![Cat Wheel](https://img.shields.io/badge/For-Cats-orange?style=flat-square)
![Arduino](https://img.shields.io/badge/Platform-Arduino-blue?style=flat-square)

## Features

### Real-Time Tracking
- **Current Speed**: Live speed display in MPH (0.1 MPH precision)
- **Miles Today**: Automatically resets at midnight
- **Smart Validation**: Filters out false readings from wheel rocking
- **Auto-Reset**: Speed automatically resets to 0 after 4 seconds of inactivity

### High Scores (All-Time Records)
- **Top Speed**: Fastest speed ever achieved
- **Total Miles**: Cumulative lifetime distance
- **Day Miles**: Total miles run during daylight hours
- **Night Miles**: Total miles run during nighttime hours
- **Persistent Storage**: All data saved to SD card, survives power loss

### Day/Night Tracking
- **Automatic Detection**: Uses RTC and sunrise/sunset calculations for Chicago
- **Location-Based**: Calculates daylight hours based on latitude/longitude
- **Seasonal Adjustment**: Automatically adapts to changing daylight hours throughout the year

### Safety Features
- **Speed Validation**: Rejects readings above 35 MPH (impossible for house cats)
- **Debouncing**: Ignores rapid back-and-forth wheel rocking (< 45ms between magnets)
- **Physical Limits**: Validates against actual cat running capabilities (~30 MPH max sprint)

## Hardware Requirements

### Components
1. **Arduino Board** (Mega recommended for memory, Uno works)
2. **3.2" TFT SPI Display** (240x320 v1.0 with SD card slot)
3. **DS3231 or DS1307 RTC Module** (I2C real-time clock)
4. **Hall Effect Sensor** (A3144, SS49E, or similar)
5. **12 Magnets** (evenly spaced on wheel rim)
6. **2 Push Buttons** (for reset functions)
7. **Micro SD Card** (for data persistence)
8. **Breadboard and Jumper Wires**
9. **CR2032 Battery** (for RTC backup)

### Cat Wheel Specifications
- **Diameter**: 118 inches (outer)
- **Circumference**: ~370.7 inches
- **Magnets**: 12 evenly spaced around the rim
- **Distance per Magnet**: ~30.89 inches

## Wiring Diagram

### TFT Display Connections (SPI)
```
TFT Pin    →    Arduino Pin
--------------------------------
VCC        →    5V
GND        →    GND
CS         →    D10
RESET      →    D8
DC/RS      →    D9
MOSI       →    D11 (hardware SPI)
SCK        →    D13 (hardware SPI)
LED        →    3.3V or 5V
MISO       →    D12 (hardware SPI)
```

### SD Card Connections
```
SD Pin     →    Arduino Pin
--------------------------------
CS         →    D4
MOSI       →    D11 (shared with TFT)
SCK        →    D13 (shared with TFT)
MISO       →    D12 (shared with TFT)
```

### Hall Sensor Connection
```
Hall Sensor Pin  →  Arduino Pin
--------------------------------
VCC              →  5V
GND              →  GND
OUT/Signal       →  D5 (with internal pull-up)
```

### Buttons
```
Component           →  Arduino Pin
--------------------------------
Daily Reset Button  →  D6 (with internal pull-up)
Trip Reset Button   →  D7 (with internal pull-up)

Button Wiring: Connect one side to GND, other side to Arduino pin
(Internal pull-ups are enabled in code)
```

## Installation

### 1. Install Required Libraries
Open Arduino IDE and install these libraries via Library Manager (Sketch → Include Library → Manage Libraries):

- **Adafruit GFX Library** (by Adafruit)
- **Adafruit ILI9341** (by Adafruit)
- **RTClib** (by Adafruit) - for real-time clock support
- **SD** (built-in, should already be available)
- **SPI** (built-in, should already be available)
- **Wire** (built-in, should already be available)

### 2. Set the Time on RTC Module
**IMPORTANT**: Before first use, you need to set the correct time on the RTC module.

1. Open `CatWheelSpeedometer.ino` in Arduino IDE
2. Find line ~140 where it says: `rtc.adjust(DateTime(2024, 1, 1, 12, 0, 0));`
3. Change to current date/time. For example, for Jan 15, 2024 at 3:30 PM:
   ```cpp
   rtc.adjust(DateTime(2024, 1, 15, 15, 30, 0));
   // Format: (year, month, day, hour, minute, second)
   ```
4. Upload the code (see step below)
5. After first upload, you can comment out this line (add `//` at the start) to prevent resetting time on each reboot

**OR** use compile-time auto-set:
```cpp
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
```

### 3. Upload the Code
1. Connect your Arduino to your computer via USB
2. Select your board type: `Tools → Board → Arduino Mega` (or Arduino Uno)
3. Select the correct port: `Tools → Port → COM_X` (or `/dev/ttyUSB0` on Linux)
4. Click the Upload button (→)

### 4. Prepare the SD Card
1. Format a micro SD card as FAT32
2. Insert it into the TFT display's SD card slot
3. The sketch will create `catwheel.txt` automatically on first run

### 5. Physical Installation

#### Magnet Placement
1. Mark 12 evenly spaced positions around the wheel rim (30° apart)
2. Attach magnets securely at each position
3. Ensure all magnets have the same pole facing outward

#### Hall Sensor Mounting
1. Mount the Hall sensor on a fixed frame near the wheel rim
2. Position it 1-10mm from the magnet path (closer is better, but avoid contact)
3. Secure firmly so it doesn't move during wheel rotation
4. Test by spinning wheel slowly and checking for detection (use Serial Monitor)

## Usage

### Display Layout

```
╔════════════════════════════╗
║  Speed                     ║
║   25.3 MPH                ║
║                            ║
║  Miles Today               ║
║    2.4 mi                 ║
║                            ║
║════════════════════════════║
║      HIGH SCORES           ║
║════════════════════════════║
║  Top Speed:    28.7 MPH    ║
║  Total Miles:  152.3 mi    ║
║  Day Miles:     95.8 mi    ║
║  Night Miles:   56.5 mi    ║
║                            ║
║ Reset Day:B1 All:B2        ║
╚════════════════════════════╝
```

### Button Functions
- **B1 (D6 - Day Reset)**: Resets today's miles to 0
- **B2 (D7 - All Reset)**: Resets ALL high scores (top speed, total miles, day/night miles) - use carefully!

### Speed Calculations
The system calculates speed using:
```
Distance per magnet = 30.89 inches
Time between magnets = measured in milliseconds
Speed (MPH) = (30.89 / time_ms) × 3,600,000 / 63,360
```

### Data Persistence
All data is automatically saved to the SD card in `catwheel.txt`:
- Today's distance (automatically resets at midnight)
- Day miles (cumulative daylight running)
- Night miles (cumulative nighttime running)
- Lifetime top speed (all-time record)
- Lifetime total miles (cumulative all-time)

**Note**: Data is saved after every wheel rotation and whenever records are broken, ensuring minimal data loss if power is interrupted. RTC keeps time even when powered off (with CR2032 battery backup).

## Troubleshooting

### RTC Issues
**Problem**: "RTC not found" error on startup
- Check I2C wiring (SDA and SCL pins)
- Verify RTC module has power (VCC and GND)
- Try scanning I2C bus (search "I2C scanner Arduino" for test code)
- Confirm you're using correct RTC library (RTClib by Adafruit)

**Problem**: Time is wrong or resets
- Set time using instructions above
- Check CR2032 battery in RTC module (should be ~3V)
- Verify you commented out `rtc.adjust()` after first upload

### Display Issues
**Problem**: Display is white/blank/garbled
- Check all wiring connections
- Verify SPI pins (MOSI, MISO, SCK) are connected correctly
- Try adjusting the `setRotation()` value in code (0, 1, 2, or 3)
- Ensure TFT_CS, TFT_DC, and TFT_RST pins match your wiring

**Problem**: Display shows "SD Card Error"
- Check SD card is properly inserted
- Verify SD_CS pin (D4) is connected correctly
- Ensure SD card is formatted as FAT32
- Try a different SD card

### Speed/Sensor Issues
**Problem**: Speed shows 0.0 even when wheel is spinning
- Check hall sensor wiring (VCC, GND, Signal)
- Verify sensor is close enough to magnets (1-10mm)
- Use Serial Monitor to debug (115200 baud)
- Check magnet polarity (all same pole facing sensor)

**Problem**: Erratic speed readings
- Magnets may not be evenly spaced
- Sensor may be too far from magnets
- Wheel may be wobbling (check mechanical alignment)
- Reduce `MAX_CAT_SPEED` if getting false high readings

**Problem**: Speed stuck at high value or not resetting
- Check `SPEED_TIMEOUT` constant (default 4000ms)
- Verify hall sensor isn't stuck LOW
- Check for loose connections

### Distance Issues
**Problem**: Distance not incrementing
- Verify all 12 magnets are in place
- Check that `magnetCounter` reaches 12 (use Serial Monitor)
- Ensure SD card is working for saves

## Customization

### Adjusting Constants
Edit these values in the code to customize behavior:

```cpp
// Wheel dimensions
const float WHEEL_DIAMETER = 118.0;          // Change if different size

// Timing
const unsigned long SPEED_TIMEOUT = 4000;    // Time until speed resets (ms)
const unsigned long MIN_MAGNET_INTERVAL = 45; // Minimum valid time between magnets (ms)

// Validation
const float MAX_CAT_SPEED = 35.0;            // Maximum believable cat speed (MPH)

// Magnets
const int MAGNET_COUNT = 12;                 // Number of magnets on wheel
```

### Changing Colors
Modify the color definitions to customize the display:

```cpp
#define COLOR_SPEED    0xF800  // Red - change to your preference
#define COLOR_DISTANCE 0x07E0  // Green
#define COLOR_DAILY    0xFFE0  // Yellow
#define COLOR_TITLE    0x07FF  // Cyan
```

Colors are in RGB565 format. Use an online converter to find color codes.

### Display Layout
The display layout can be customized by modifying the `drawStaticUI()` and update functions. Key coordinates are commented in the code.

## Technical Details

### Speed Calculation Formula
```
Speed (MPH) = (Distance / Time) × 3,600,000 / 63,360

Where:
- Distance = 30.89 inches (per magnet pass)
- Time = milliseconds between magnet detections
- 3,600,000 = converts ms to hours
- 63,360 = inches per mile
```

### Validation Logic
1. **Minimum Time Check**: Rejects readings with < 45ms between magnets (prevents false triggers from wheel rocking)
2. **Maximum Speed Check**: Rejects readings > 35 MPH (physically impossible for house cats)
3. **Debounce**: 10ms button debounce prevents multiple triggers

### Memory Usage
- **SRAM**: ~1.2KB (varies with display library)
- **Flash**: ~22KB (fits comfortably on Arduino Uno's 32KB)

Recommended: Arduino Uno or better (Mega/Leonardo/Nano)

## Serial Monitor Debugging

To view debug messages:
1. Open Serial Monitor in Arduino IDE
2. Set baud rate to 9600
3. You'll see messages like:
   - "Cat Wheel Speedometer Starting..."
   - "SD Card initialized"
   - "Data loaded from SD"
   - "Rejected speed: XX MPH (too fast)" - when false readings are filtered

## Credits

Created for indoor cat exercise and enrichment.

## License

Open source - feel free to modify and share!

## Support

For issues or questions:
- Check the Troubleshooting section above
- Review wiring connections
- Verify component compatibility
- Check Serial Monitor for debug messages

Happy tracking! 🐱💨
