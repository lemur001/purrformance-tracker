# Complete Wiring Guide - Your Exact Components

## 🎯 You Have Everything - Let's Build It!

### Your Components Checklist:
- ✅ Elegoo UNO R3
- ✅ 3.2" TFT SPI 240x320 Touchscreen Display (with SD slot)
- ✅ DS3231 RTC Module (with CR2032 battery installed)
- ✅ Hall Magnetic Sensor
- ✅ 12 Magnets (already on cat wheel)
- ✅ 2 Push Buttons
- ✅ 830 Tie-Points Breadboard
- ✅ Jumper Wires
- ✅ Micro SD Card
- ✅ 9V 1A Adapter

**Estimated Build Time**: 30-45 minutes

---

## 📋 Complete Pin Connection Table

### TFT Display → Arduino UNO

| Your TFT Pin | Wire To | Arduino Pin | Notes |
|--------------|---------|-------------|-------|
| **VCC** | Power Rail | **5V** | Red wire recommended |
| **GND** | Ground Rail | **GND** | Black wire |
| **CS** | Digital | **D10** | Display chip select |
| **RESET** | Digital | **D8** | Display reset |
| **DC** | Digital | **D9** | Data/Command |
| **SDI(MOSI)** | SPI | **D11** | Data to display |
| **SCK** | SPI | **D13** | SPI clock |
| **LED** | Digital PWM | **D3** | Backlight (brightness controlled by pot) |
| **SDO(MISO)** | SPI | **D12** | Data from display |

**IGNORE these pins on your display:**
- T_IRQ, T_DO, T_DI, T_CS, T_CLK (touch screen - not used)

### SD Card → Arduino UNO

| Your SD Pin | Wire To | Arduino Pin | Notes |
|-------------|---------|-------------|-------|
| **SD_CS** | Digital | **D4** | SD card chip select |
| **SD_MOSI** | Shared | **D11** | Same as TFT MOSI |
| **SD_SCK** | Shared | **D13** | Same as TFT SCK |
| **SD_MISO** | Shared | **D12** | Same as TFT MISO |

**Note**: SD card shares SPI pins with TFT display. That's normal and correct!

### DS3231 RTC → Arduino UNO

| RTC Pin | Wire To | Arduino Pin | Notes |
|---------|---------|-------------|-------|
| **VCC** | Power Rail | **5V** | Red wire |
| **GND** | Ground Rail | **GND** | Black wire |
| **SDA** | I2C Data | **A4** | Blue/Green wire |
| **SCL** | I2C Clock | **A5** | Yellow/White wire |

**Note**: If your RTC has a 4th pin (SQW or 32K), leave it unconnected.

### Hall Sensor → Arduino UNO

| Hall Sensor Pin | Wire To | Arduino Pin | Notes |
|-----------------|---------|-------------|-------|
| **S** (Signal) | Digital | **D5** | Yellow wire |
| **+** (VCC) | Power Rail | **5V** | Red wire |
| **-** (GND) | Ground Rail | **GND** | Black wire |

**Note**: Your Hall sensor might have labels "S", "VCC", "GND" or just "+", "-", "S"

### Buttons → Arduino UNO

| Button | Wire 1 To | Wire 2 To | Function |
|--------|-----------|-----------|----------|
| **Button 1** | **D6** | **GND** | Reset today's miles |
| **Button 2** | **D7** | **GND** | Reset all high scores |

**Note**: Internal pullup resistors are enabled in code - no external resistors needed!

### 10k Potentiometer (Brightness Control) → Arduino UNO

| Pot Pin | Wire To | Arduino Pin | Notes |
|---------|---------|-------------|-------|
| **Pin 1** (one end) | Power Rail | **5V** | Red wire |
| **Pin 2** (wiper/middle) | Analog | **A0** | Signal wire |
| **Pin 3** (other end) | Ground Rail | **GND** | Black wire |

**Note**: Turning the knob adjusts display brightness from dim to bright!

### Power Connections

| Arduino Pin | Wire To | What Gets Power |
|-------------|---------|-----------------|
| **5V** | Breadboard + Rail | All VCC connections |
| **GND** | Breadboard - Rail | All GND connections |
| **Barrel Jack** | 9V 1A Adapter | Powers entire system |

---

## 🔌 Step-by-Step Wiring Instructions

### Step 1: Set Up Your Breadboard Power Rails

1. **Place your Arduino UNO** on the table (don't plug into breadboard yet)
2. **Place your 830-point breadboard** in front of you
3. **Identify the power rails**:
   - Red line = + (positive)
   - Blue/Black line = - (negative/ground)

4. **Connect Arduino to breadboard power rails**:
   ```
   Arduino 5V pin → Red jumper → Breadboard + rail
   Arduino GND pin → Black jumper → Breadboard - rail
   ```

**Pro Tip**: Connect the + and - rails on both sides of the breadboard together (top and bottom) for easier wiring.

---

### Step 2: Wire the TFT Display

Your TFT display has a row of pins. Looking at the pin labels:

```
Pin Order on Display (left to right):
VCC, GND, CS, RESET, DC, SDI(MOSI), SCK, LED, SDO(MISO), T_IRQ, T_DO, T_DIN, T_CS, T_CLK
```

**Wire these pins (ignore the T_* pins):**

1. **VCC** → Breadboard + rail (red wire)
2. **GND** → Breadboard - rail (black wire)
3. **CS** → Arduino **D10** (orange wire)
4. **RESET** → Arduino **D8** (yellow wire)
5. **DC** → Arduino **D9** (green wire)
6. **SDI(MOSI)** → Arduino **D11** (blue wire)
7. **SCK** → Arduino **D13** (purple/white wire)
8. **LED** → Arduino **D3** (white wire) *brightness controlled by potentiometer*
9. **SDO(MISO)** → Arduino **D12** (gray wire)

**Leave unconnected**: T_IRQ, T_DO, T_DIN, T_CS, T_CLK

---

### Step 3: Wire the SD Card Pins

On the BACK of your TFT display, you'll see SD card pins:

```
SD Card Pins (usually labeled on back):
SD_CS, SD_MOSI, SD_MISO, SD_SCK
```

**Wire these pins:**

1. **SD_CS** → Arduino **D4** (any color wire)
2. **SD_MOSI** → Arduino **D11** (share with TFT MOSI - can use same row on breadboard)
3. **SD_MISO** → Arduino **D12** (share with TFT MISO)
4. **SD_SCK** → Arduino **D13** (share with TFT SCK)

**Tip**: Since MOSI, MISO, and SCK are shared between TFT and SD card, you can:
- Connect both to the same breadboard row, OR
- Connect both wires directly to the same Arduino pin

---

### Step 4: Wire the DS3231 RTC Module

Your DS3231 module has 4-6 pins. You only need 4:

```
DS3231 Pin Labels:
VCC (or 5V), GND, SDA, SCL
(ignore: SQW, 32K if present)
```

**Wire these pins:**

1. **VCC** → Breadboard + rail (red wire)
2. **GND** → Breadboard - rail (black wire)
3. **SDA** → Arduino **A4** (blue or green wire)
4. **SCL** → Arduino **A5** (yellow or white wire)

**Check**: Make sure CR2032 battery is inserted on the back of the RTC module!

---

### Step 5: Wire the Hall Sensor

Your Hall Magnetic Sensor has 3 pins:

```
Hall Sensor Labels (your sensor might vary):
S (Signal), + or VCC, - or GND
OR
OUT, VCC, GND
```

**Wire these pins:**

1. **S or OUT** → Arduino **D5** (yellow wire)
2. **+ or VCC** → Breadboard + rail (red wire)
3. **- or GND** → Breadboard - rail (black wire)

**Physical Placement**: Mount this sensor near your cat wheel rim (1-5mm away from magnet path). Don't wire it permanently yet - test first!

---

### Step 6: Wire the 10k Potentiometer (Brightness Control)

Your 10k potentiometer controls the display brightness. It has 3 pins.

```
Potentiometer (looking at it from the front with shaft up):

    Pin 1     Pin 2      Pin 3
    (left)   (middle)   (right)
      │         │          │
      │         │          │
   to 5V     to A0      to GND
```

**Wire these pins:**

1. **Pin 1** (left pin when facing front) → Breadboard + rail (red wire)
2. **Pin 2** (middle pin/wiper) → Arduino **A0** (any color wire)
3. **Pin 3** (right pin) → Breadboard - rail (black wire)

**How it works:**
- Turn knob fully counter-clockwise = dimmer display
- Turn knob fully clockwise = brighter display
- The code ensures display never goes completely off (minimum brightness)

**Tip**: Orient the pot so that clockwise = brighter. If it's backwards, just swap pins 1 and 3.

---

### Step 7: Wire the Buttons

You have 5 buttons - you only need 2. They're small tactile switches.

**Button 1 (Reset Today's Miles):**
```
Arduino D6 ───┐
              │
           Button 1
              │
         GND rail ───┘
```

**How to wire:**
1. Push button into breadboard (straddle the center gap)
2. One side of button → Arduino **D6** (any color wire)
3. Other side of button → Breadboard - rail (black wire)

**Button 2 (Reset All High Scores):**
```
Arduino D7 ───┐
              │
           Button 2
              │
         GND rail ───┘
```

**How to wire:**
1. Push button into breadboard (straddle the center gap)
2. One side of button → Arduino **D7** (any color wire)
3. Other side of button → Breadboard - rail (black wire)

**Note**: If your buttons have 4 pins, use diagonal pins (pins 1 and 3, or 2 and 4).

---

### Step 8: Insert the SD Card

1. **Format your SD card** as FAT32 (use computer)
2. **Eject safely** from computer
3. **Insert into the SD slot** on the back of your TFT display
4. Push until it clicks (it should be flush or slightly recessed)

---

## 🔍 Visual Wire Reference

### Color Coding Suggestion (makes debugging easier):

| Wire Color | Common Use |
|------------|------------|
| **Red** | 5V / Power |
| **Black** | GND / Ground |
| **Orange** | TFT CS (D10) |
| **Yellow** | TFT RESET (D8), Hall Sensor Signal (D5) |
| **Green** | TFT DC (D9), RTC SDA (A4) |
| **Blue** | TFT MOSI (D11) |
| **Purple/White** | TFT SCK (D13) |
| **Gray** | TFT MISO (D12) |
| **Brown** | SD CS (D4) |
| **White** | TFT LED (D3), RTC SCL (A5) |
| **Any color** | Pot wiper to A0 |

---

## ⚡ Power Connection

**Option 1: USB Power (for testing)**
```
Computer USB → Arduino USB port
```
- Pros: Easy for testing and uploading code
- Cons: Tethered to computer

**Option 2: Wall Adapter (for permanent installation)**
```
9V 1A Adapter → Arduino barrel jack
```
- Pros: Standalone operation, can place anywhere
- Cons: Needs nearby outlet

**Recommendation**: Use USB for initial testing, then switch to 9V adapter once everything works.

---

## ✅ Pre-Upload Checklist

Before uploading code, verify these connections:

### Power Connections:
- [ ] Arduino 5V → Breadboard + rail
- [ ] Arduino GND → Breadboard - rail
- [ ] TFT VCC → Breadboard + rail
- [ ] TFT GND → Breadboard - rail
- [ ] TFT LED → Breadboard + rail
- [ ] RTC VCC → Breadboard + rail
- [ ] RTC GND → Breadboard - rail
- [ ] Hall VCC → Breadboard + rail
- [ ] Hall GND → Breadboard - rail

### TFT Display:
- [ ] CS → D10
- [ ] RESET → D8
- [ ] DC → D9
- [ ] SDI(MOSI) → D11
- [ ] SCK → D13
- [ ] LED → D3 (NOT 5V!)
- [ ] SDO(MISO) → D12

### SD Card:
- [ ] SD_CS → D4
- [ ] SD_MOSI → D11 (shared with TFT)
- [ ] SD_SCK → D13 (shared with TFT)
- [ ] SD_MISO → D12 (shared with TFT)
- [ ] SD card inserted into slot

### RTC Module:
- [ ] SDA → A4
- [ ] SCL → A5
- [ ] CR2032 battery installed

### Hall Sensor:
- [ ] Signal → D5
- [ ] VCC → + rail
- [ ] GND → - rail

### Buttons:
- [ ] Button 1: one side → D6, other side → GND
- [ ] Button 2: one side → D7, other side → GND

### Potentiometer (Brightness):
- [ ] Left pin → 5V rail
- [ ] Middle pin (wiper) → A0
- [ ] Right pin → GND rail

### Safety Checks:
- [ ] No wires crossing or touching that shouldn't
- [ ] No bare wire exposed (all connections secure)
- [ ] 5V and GND rails not connected together (that would be a short!)
- [ ] All wires firmly inserted into breadboard

---

## 🎬 First Test: Upload and Run

### 1. Install Arduino Libraries

Open Arduino IDE, go to **Sketch → Include Library → Manage Libraries**

Install these:
- **Adafruit GFX Library** (search "Adafruit GFX")
- **Adafruit ILI9341** (search "Adafruit ILI9341")
- **RTClib** (search "RTClib", by Adafruit)

The others (SD, SPI, Wire) are built-in.

### 2. Set the RTC Time

1. Open `CatWheelSpeedometer.ino` in Arduino IDE
2. Find line ~132: `rtc.adjust(DateTime(2024, 1, 1, 12, 0, 0));`
3. Change to current date/time. **Example for November 9, 2025 at 8:30 PM:**
   ```cpp
   rtc.adjust(DateTime(2025, 11, 9, 20, 30, 0));
   // Format: (year, month, day, hour(24h), minute, second)
   ```

### 3. Upload the Code

1. Connect Arduino to computer via USB
2. In Arduino IDE:
   - **Tools → Board** → "Arduino Uno"
   - **Tools → Port** → Select your Arduino's port (COM# on Windows, /dev/tty* on Mac/Linux)
3. Click **Upload** button (→)
4. Wait for "Done uploading"

### 4. Watch the Display

**You should see:**
1. "Initializing System..." message (brightness controlled by pot position)
2. If RTC found: "RTC OK" in Serial Monitor (Tools → Serial Monitor)
3. If SD card found: "SD Card initialized" in Serial Monitor
4. Main display appears with:
   - Speed: 0.0 MPH
   - Miles Today: 0.0 mi
   - HIGH SCORES (all zeros initially)

**Test brightness control:**
- Turn potentiometer knob
- Display should get brighter/dimmer smoothly
- Display should never go completely black (min brightness = 20/255)

**If you see errors**, check the Troubleshooting section below.

### 5. Test the Hall Sensor

1. Open **Serial Monitor** (Tools → Serial Monitor, set to 9600 baud)
2. **Manually spin your cat wheel slowly**
3. **Watch Serial Monitor** for messages like:
   - "Data saved to SD"
   - Speed calculations
4. **Watch the display** - speed should update!

### 6. Test the Buttons

- **Press Button 1 (D6)**: Should see "Today's miles reset" in Serial Monitor
- **Press Button 2 (D7)**: Should see "All high scores reset" in Serial Monitor

---

## 🚨 Troubleshooting

### Display Shows Nothing / White Screen

**Check these:**
1. TFT power connections (VCC and GND)
2. TFT LED connection (backlight)
3. All SPI connections (MOSI, MISO, SCK)
4. CS, DC, RESET pins (D10, D9, D8)

**Try:**
- Swap MOSI and MISO (easy to mix up)
- Check all connections are firm in breadboard

### "RTC not found" Error

**Check these:**
1. RTC power (VCC and GND)
2. SDA → A4 (not A5)
3. SCL → A5 (not A4)
4. Battery installed in RTC

**Try:**
- Swap SDA and SCL (common mistake)
- Upload an I2C scanner sketch to find RTC address

### "SD Card failed" Error

**Check these:**
1. SD card is inserted fully
2. SD card is FAT32 formatted
3. SD_CS → D4
4. Shared pins (MOSI, MISO, SCK) connected to both TFT and SD

**Try:**
- Remove and reinsert SD card
- Try different SD card
- Format SD card as FAT32

### Hall Sensor Not Detecting

**Check these:**
1. Sensor too far from magnets (should be 1-5mm)
2. Signal wire → D5
3. Magnets are strong enough (neodymium)

**Try:**
- Move sensor closer to wheel
- Check sensor polarity (some sensors only detect one pole)
- Test with a handheld magnet

### Buttons Don't Work

**Check these:**
1. Button connections (one side to D6/D7, other to GND)
2. Button is momentary type (not latching)

**Try:**
- Press and hold for 1 second
- Try different button from your kit
- Check button orientation (4-pin buttons need diagonal pins)

---

## 🎨 Final Assembly Tips

### After Everything Works:

1. **Comment out the RTC time-set line** (add `//` at the start):
   ```cpp
   // rtc.adjust(DateTime(2025, 11, 9, 20, 30, 0));
   ```
   This prevents the clock from resetting every time you power on.

2. **Mount the Hall sensor permanently**:
   - Position 1-5mm from magnet path
   - Use hot glue, zip ties, or mounting tape
   - Make sure it doesn't touch moving parts

3. **Mount the display** where you can see it:
   - Near the cat wheel
   - At eye level
   - Protected from cat paws!

4. **Organize wires**:
   - Use zip ties or wire clips
   - Keep wires away from cat wheel
   - Leave some slack for adjustments

5. **Consider an enclosure**:
   - Protects from curious cats
   - Looks cleaner
   - Prevents accidental disconnections

---

## 📊 What to Expect

### First Day of Use:
- Speed will jump around as cats explore
- Distance will slowly accumulate
- Top speed will be set
- Day/Night miles will track based on Chicago daylight

### After a Week:
- You'll see patterns (morning runner? night owl?)
- High scores stabilize
- You'll know your cats' personalities!

### Typical Cat Performance:
- **Walking**: 3-8 MPH
- **Running**: 10-20 MPH
- **Sprinting**: 20-30 MPH (short bursts)
- **Daily distance**: 0.5-3 miles (very active cats)

---

## 🎉 You're Done!

Your cat wheel speedometer is complete! Have fun tracking your cats' exercise and discovering when they're most active.

**Need help?** Check the main README.md or post issues on GitHub.

**Happy tracking!** 🐱💨
