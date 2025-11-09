# Cat Wheel Speedometer & Odometer

A comprehensive Arduino-based speedometer and odometer system for cat exercise wheels, featuring real-time speed tracking, trip odometer, daily leaderboards, and persistent data storage.

![Cat Wheel](https://img.shields.io/badge/For-Cats-orange?style=flat-square)
![Arduino](https://img.shields.io/badge/Platform-Arduino-blue?style=flat-square)

## Features

### Real-Time Tracking
- **Current Speed**: Live speed display in MPH (0.1 MPH precision)
- **Trip Odometer**: Current session distance in miles (0.1 mile precision)
- **Smart Validation**: Filters out false readings from wheel rocking
- **Auto-Reset**: Speed automatically resets to 0 after 4 seconds of inactivity

### Daily Leaderboard
- **Daily Top Speed**: Highest speed achieved today
- **Daily Total Miles**: Total distance covered today
- **Persistent Storage**: Data saved to SD card, survives power loss
- **Easy Reset**: Dedicated buttons for daily and trip resets

### Safety Features
- **Speed Validation**: Rejects readings above 35 MPH (impossible for house cats)
- **Debouncing**: Ignores rapid back-and-forth wheel rocking (< 45ms between magnets)
- **Physical Limits**: Validates against actual cat running capabilities (~30 MPH max sprint)

## Hardware Requirements

### Components
1. **Arduino Board** (Uno or Mega recommended for better memory)
2. **3.2" TFT SPI Display** (240x320 v1.0 with SD card slot)
3. **Hall Effect Sensor** (A3144, SS49E, or similar)
4. **12 Magnets** (evenly spaced on wheel rim)
5. **2 Push Buttons** (for reset functions)
6. **Micro SD Card** (for data persistence)
7. **Breadboard and Jumper Wires**
8. **Pull-up Resistors** (if not using internal pull-ups)

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
- **SD** (built-in, should already be available)
- **SPI** (built-in, should already be available)

### 2. Upload the Code
1. Connect your Arduino to your computer via USB
2. Open `CatWheelSpeedometer.ino` in Arduino IDE
3. Select your board type: `Tools → Board → Arduino Uno` (or your board)
4. Select the correct port: `Tools → Port → COM_X` (or `/dev/ttyUSB0` on Linux)
5. Click the Upload button (→)

### 3. Prepare the SD Card
1. Format a micro SD card as FAT32
2. Insert it into the TFT display's SD card slot
3. The sketch will create `catwheel.txt` automatically on first run

### 4. Physical Installation

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
║      CAT WHEEL             ║
║════════════════════════════║
║  Current Speed             ║
║                            ║
║     25.3 MPH              ║
║                            ║
║  Trip Miles                ║
║                            ║
║      2.4 mi               ║
║                            ║
║════════════════════════════║
║      DAILY BEST            ║
║                            ║
║  Top Speed:    28.7 MPH    ║
║                            ║
║  Total Miles:   5.2 mi     ║
║                            ║
║  D6:Daily D7:Trip Reset    ║
╚════════════════════════════╝
```

### Button Functions
- **D6 (Daily Reset)**: Resets daily top speed and daily distance to 0
- **D7 (Trip Reset)**: Resets current trip odometer to 0

### Speed Calculations
The system calculates speed using:
```
Distance per magnet = 30.89 inches
Time between magnets = measured in milliseconds
Speed (MPH) = (30.89 / time_ms) × 3,600,000 / 63,360
```

### Data Persistence
All data is automatically saved to the SD card in `catwheel.txt`:
- Daily top speed (updated when beaten)
- Daily total distance (updated every rotation)
- Lifetime top speed (all-time record)
- Lifetime total miles (cumulative)

**Note**: Data is saved after every wheel rotation and whenever records are broken, ensuring minimal data loss if power is interrupted.

## Troubleshooting

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
