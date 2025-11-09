# Cat Wheel Speedometer - Parts List

## Required Components

### Core Components

| Item | Specification | Quantity | Notes | Est. Price |
|------|--------------|----------|-------|------------|
| Arduino Uno | R3 or compatible | 1 | Mega/Leonardo also work | $20-25 |
| TFT LCD Display | 3.2" SPI 240x320 v1.0 with SD slot | 1 | Must have SD card slot | $15-20 |
| Hall Effect Sensor | A3144, A3141, SS49E, or similar | 1 | Digital output type | $1-3 |
| Neodymium Magnets | 5-10mm diameter | 12 | Strong enough to trigger sensor | $5-10 |
| Push Buttons | Momentary tactile switch | 2 | 6mm tactile switches work well | $1-2 |
| Micro SD Card | 1GB or larger, FAT32 | 1 | Any size works, smaller is fine | $3-5 |

### Wiring & Mounting

| Item | Specification | Quantity | Notes | Est. Price |
|------|--------------|----------|-------|------------|
| Breadboard | Half or full size | 1 | For prototyping | $3-5 |
| Jumper Wires | Male-to-male, male-to-female | 20-30 | Various lengths | $5-8 |
| USB Cable | Type A to Type B | 1 | For Arduino programming/power | $3-5 |
| Power Supply | 5V 2A USB or 7-12V DC | 1 | Optional if USB powered | $5-10 |

### Optional but Recommended

| Item | Purpose | Est. Price |
|------|---------|------------|
| Project Enclosure | Protect electronics from cats | $10-15 |
| Double-sided Tape | Mount magnets securely | $3-5 |
| Wire Clips | Cable management | $2-5 |
| Heat Shrink Tubing | Protect connections | $3-5 |

## Total Estimated Cost
**Minimum**: ~$55-70 (with existing Arduino and tools)
**Complete Kit**: ~$75-100 (everything new)

## Where to Buy

### Online Retailers
- **Amazon**: Good for complete kits and fast shipping
- **AliExpress**: Cheapest, but slower shipping (2-4 weeks)
- **Adafruit**: Quality components, good support
- **SparkFun**: Educational focus, reliable
- **eBay**: Mix of new and surplus parts

### Local Options
- **Electronics Stores**: Best Buy, Micro Center (if available)
- **Hobby Shops**: Some carry Arduino components
- **University Surplus**: Often have deals on electronics

## Detailed Component Specifications

### Arduino Board
**Recommended Models:**
- Arduino Uno R3 (most common, 32KB flash, 2KB RAM)
- Arduino Mega 2560 (more pins, 256KB flash, 8KB RAM - better for expansion)
- Arduino Leonardo (built-in USB, 32KB flash)
- Arduino Nano (compact, same specs as Uno)

**Why Uno is recommended**: Best balance of price, availability, and capability for this project.

### TFT Display
**Exact Specification**: 3.2 inch TFT LCD SPI 240x320 v1.0

**Critical Features:**
- SPI interface (NOT parallel/8-bit)
- ILI9341 or compatible controller
- Built-in SD card slot (MUST HAVE for data persistence)
- 240x320 resolution
- 5V tolerant pins

**Common Sellers**: Look for "3.2 TFT SPI ILI9341 SD" on Amazon/eBay

**WARNING**: There are many similar displays. Verify:
- Has SPI (not 8-bit parallel)
- Has SD card slot on back
- Includes SD card libraries/examples

### Hall Effect Sensor
**Recommended Types:**
- **A3144**: Digital latching type, very common
- **SS49E**: Linear type with digital output
- **A3141**: Similar to A3144
- **US1881**: Alternative digital type

**Key Specifications:**
- Digital output (HIGH/LOW)
- 5V operating voltage
- Sensitivity: 35-200 Gauss typical
- Output: Open collector or push-pull

**Form Factor**: TO-92 package (3 pins, looks like a small transistor)

**Pinout** (usually):
```
   Flat side facing you
      _____
     |  o  |  ← Top view
     |_____|
      | | |
      1 2 3

Pin 1: VCC (5V)
Pin 2: GND
Pin 3: OUT (Signal)

NOTE: Always check datasheet, some vary!
```

### Magnets
**Specifications:**
- **Type**: Neodymium (NdFeB) - strongest option
- **Size**: 5-10mm diameter, 2-5mm thick
- **Grade**: N35 or better (N42, N52 stronger but pricier)
- **Shape**: Disc or cylinder
- **Quantity**: 12 pieces (plus 1-2 spares)

**Mounting**:
- Use strong adhesive (epoxy, super glue, or double-sided tape)
- Ensure consistent polarity (all North or all South facing sensor)
- Space evenly: 360° / 12 = 30° apart

**Safety**: Keep away from small children, credit cards, and electronic devices.

### Buttons
**Specifications:**
- **Type**: Momentary tactile push button
- **Size**: 6x6mm or 12x12mm
- **Mounting**: Through-hole (4 pins) or 2-pin
- **Voltage**: 5V+ rating
- **Current**: 50mA+ rating

**Common Types:**
- 6x6mm tactile switches (tiny, breadboard friendly)
- 12x12mm tactile switches (easier to press)
- Panel mount push buttons (for enclosures)

### Micro SD Card
**Specifications:**
- **Capacity**: 1GB to 32GB (bigger not needed)
- **Format**: Must be FAT32
- **Speed**: Class 4 or better (speed not critical)
- **Type**: Micro SD or Micro SDHC

**Note**: Cards larger than 32GB are usually exFAT and may need reformatting to FAT32.

## Tools Needed

### Required
- Computer (Windows, Mac, or Linux)
- Arduino IDE software (free download)
- USB port for Arduino connection

### Helpful
- Wire strippers
- Small screwdriver
- Multimeter (for troubleshooting)
- Helping hands/clips (for soldering)
- Soldering iron (if making permanent connections)

## Alternative Components

### Budget Alternatives
- **Generic Arduino clones**: $5-10 (work identically to official Arduino)
- **Smaller display**: 2.8" or 2.4" TFT (cheaper, but less screen space)
- **Analog Hall sensor**: Can use any Hall sensor, modify code for analog reading

### Upgrade Options
- **Arduino Mega**: More memory for expanded features
- **Real-time clock module**: Automatic daily resets at midnight
- **Bluetooth module**: Send data to phone app
- **Larger display**: 4.3" or 7" TFT for easier viewing

## Shopping List Template

Copy this checklist when shopping:

```
[ ] Arduino Uno R3 board
[ ] 3.2" TFT SPI 240x320 display with SD slot
[ ] Hall effect sensor (A3144 or similar)
[ ] 12+ neodymium magnets (5-10mm)
[ ] 2 tactile push buttons
[ ] Micro SD card (FAT32 formatted)
[ ] Breadboard (half size minimum)
[ ] 20-30 jumper wires (male-male and male-female)
[ ] USB cable (Type A to Type B)
[ ] (Optional) 5V power supply
[ ] (Optional) Project enclosure
[ ] (Optional) Mounting tape/adhesive
```

## Pre-Built Kit Option

Currently, there are no pre-built kits specifically for this project. However, you can:

1. **Arduino Starter Kit**: Buy a starter kit that includes Arduino, breadboard, wires, buttons ($40-60)
2. **Add Separately**: TFT display, Hall sensor, magnets (~$20-30)
3. **Total**: ~$60-90 with more components than needed

**Recommended Starter Kits**:
- Elegoo Uno Project Complete Starter Kit
- Arduino Official Starter Kit
- LAFVIN Super Starter Kit

These include resistors, LEDs, and other components useful for future projects.

## Notes

- Prices are USD estimates as of 2024 and vary by region
- Many components available cheaper in bulk (if building multiple units)
- Check local electronics stores for same-day pickup
- Used/surplus components work fine if tested
- Always buy a few extra small components (magnets, sensors) as spares
