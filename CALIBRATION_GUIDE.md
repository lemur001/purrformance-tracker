# Cat Wheel Speedometer - Calibration & Testing Guide

## Overview
This guide helps you calibrate, test, and optimize your cat wheel speedometer for accurate readings.

## Pre-Calibration Checklist

Before starting calibration, ensure:
- [ ] All wiring is complete and verified
- [ ] Code successfully uploaded to Arduino
- [ ] Display shows initial screen properly
- [ ] SD card inserted and recognized
- [ ] Magnets attached to wheel (all 12)
- [ ] Hall sensor mounted near wheel rim

## Step 1: Verify Wheel Measurements

### Measure Your Wheel Diameter

Your code is set for 118 inches diameter. Verify this is correct:

1. **Method A: Direct Measurement**
   - Measure straight across the wheel at the widest point
   - Measure on the OUTSIDE edge where magnets are mounted
   - Use a measuring tape or string + ruler

2. **Method B: Calculate from Circumference**
   - Wrap a string around the outer rim (one complete rotation)
   - Measure the string length (circumference)
   - Diameter = Circumference ÷ 3.14159

**If your diameter is different:**
Edit this line in the code:
```cpp
const float WHEEL_DIAMETER = 118.0;  // Change to your measurement
```

### Verify Magnet Count

Count your magnets: Should be exactly **12 magnets**

**If different:**
Edit this line:
```cpp
const int MAGNET_COUNT = 12;  // Change to your count
```

**Important**: Magnets must be evenly spaced!
- 12 magnets = 30° apart (360° ÷ 12)
- Use a protractor or mark wheel into equal sections

## Step 2: Hall Sensor Positioning

The sensor position is critical for reliable detection.

### Finding the Optimal Distance

1. **Start Position**: Mount sensor 5mm from magnet path
2. **Upload Code**: Upload the main sketch with Serial debugging
3. **Open Serial Monitor**: Set baud rate to 9600
4. **Spin Wheel Slowly**: Watch Serial Monitor for detection messages
5. **Adjust Distance**:
   - **Too far** (>10mm): No detections, move closer
   - **Too close** (<1mm): May get false triggers, move slightly away
   - **Optimal** (1-5mm): Clean, consistent detections

### Testing Detection

**Manual Test:**
```
1. Slowly rotate wheel by hand (very slow)
2. Watch Serial Monitor
3. Each magnet should trigger exactly once
4. Count detections for one full rotation
   Expected: 12 detections

If not detecting all 12:
- Sensor too far from magnets
- Some magnets mounted incorrectly (wrong pole)
- Sensor wiring issue (check connections)
```

**Visual Indicator Test:**
As wheel spins, watch the display:
- Speed should change from 0.0 to a value
- After wheel stops, speed should return to 0.0 after 4 seconds

### Alignment Tips

**Horizontal Alignment:**
```
Side View:
                Wheel Rim
                    │
                    │ ◄── Magnet path
    ╔═══════╗      │
    ║ Hall  ║ ◄─── │ ─── 1-5mm gap
    ║ Sensor║      │
    ╚═══════╝      │
                    │

Sensor should point directly at magnet path
```

**Vertical Alignment:**
```
Top View:

    Wheel Rim → ╱  ╲
               │    │
    Magnets → ╲  ╱  ← Sensor here (perpendicular to rim)

Sensor face should be perpendicular to wheel rotation
```

## Step 3: Speed Calibration

### Test Speed Calculations

**Manual Spin Test:**
1. Mark one magnet with tape (reference point)
2. Spin wheel at a consistent, slow speed
3. Time how long it takes to complete 10 rotations
4. Calculate expected speed:

```
Example Calculation:
- 10 rotations in 30 seconds = 0.33 rotations/second
- Circumference = 370.7 inches
- Distance = 370.7 × 10 = 3,707 inches
- Time = 30 seconds = 0.00833 hours
- Speed = 3,707 ÷ 63,360 ÷ 0.00833 = 7.0 MPH

Compare to display reading - should be close!
```

### Validate Speed Limits

The code rejects speeds above 35 MPH (too fast for cats).

**Test False Reading Protection:**
1. Quickly spin wheel by hand (very fast)
2. Watch Serial Monitor for "Rejected speed" messages
3. This confirms the validation is working

**If you see many rejections during normal cat use:**
- Your max speed threshold may be too low
- Check actual cat speeds first (most cats: 15-25 MPH max)
- Adjust `MAX_CAT_SPEED` constant if needed:
```cpp
const float MAX_CAT_SPEED = 35.0;  // Increase if needed
```

### Calibrate Minimum Interval

Prevents false triggers from wheel rocking.

**Current setting**: 45ms minimum between magnets

**Test rocking protection:**
1. Gently rock wheel back and forth (don't spin)
2. Watch if display shows speed readings
3. **Expected**: Should NOT show speed (rejects rapid back-and-forth)
4. **If it does show speed**: Increase `MIN_MAGNET_INTERVAL`:
```cpp
const unsigned long MIN_MAGNET_INTERVAL = 50;  // Try 50, 60, etc.
```

**Calculate minimum interval for your needs:**
```
At max expected speed (e.g., 30 MPH):
- 30 MPH = 1,900,800 inches/hour = 528 inches/second
- Time per magnet (30.89 inches) = 30.89 ÷ 528 = 0.0585 seconds = 58ms

Set MIN_MAGNET_INTERVAL to ~50-70% of this:
MIN_MAGNET_INTERVAL = 35-40ms (allows fast speeds)

For more aggressive filtering (if rocking is an issue):
MIN_MAGNET_INTERVAL = 50-60ms
```

## Step 4: Distance Calibration

### Verify Odometer Accuracy

**Method: Known Distance Test**

1. Reset trip odometer (press D7 button)
2. Manually rotate wheel exactly 10 full rotations
3. Check trip odometer reading

**Expected calculation:**
```
Wheel circumference = 370.7 inches
10 rotations = 3,707 inches
In miles = 3,707 ÷ 63,360 = 0.0585 miles = 0.1 miles (rounded)

Display should show: ~0.1 miles
```

**If reading is off:**
- Recount magnets (should be exactly 12)
- Verify diameter measurement
- Check that all 12 magnets are detected (Serial Monitor during rotation)

### Long-Term Accuracy Test

**For serious calibration enthusiasts:**

1. Measure wheel circumference with string (very carefully)
2. Calculate exact distance per rotation:
   ```
   Distance (miles) = Measured Circumference (inches) ÷ 63,360
   ```
3. Compare to code's calculation
4. Adjust `WHEEL_DIAMETER` constant if needed for precision

## Step 5: Display Calibration

### Verify Display Values

Check that all displays update correctly:

**Current Speed:**
- [ ] Shows 0.0 when wheel stopped
- [ ] Updates in real-time when spinning
- [ ] Returns to 0.0 after 4 seconds of no motion
- [ ] Shows to 0.1 MPH precision (e.g., 12.3 MPH)

**Trip Odometer:**
- [ ] Starts at 0.0
- [ ] Increments after each full rotation
- [ ] Shows to 0.1 mile precision
- [ ] Resets with D7 button

**Daily Top Speed:**
- [ ] Shows highest speed achieved
- [ ] Only updates when exceeded
- [ ] Persists through power cycles (SD card)
- [ ] Resets with D6 button

**Daily Distance:**
- [ ] Accumulates total miles
- [ ] Continues from previous session (SD card)
- [ ] Resets with D6 button

### Test SD Card Persistence

**Power Cycle Test:**
1. Run wheel, achieve some speed and distance
2. Note daily top speed and daily distance
3. Power off Arduino completely (disconnect USB)
4. Wait 10 seconds
5. Power on Arduino
6. **Expected**: Daily stats should restore from SD card

**If data is lost:**
- Check SD card is inserted
- Verify SD card formatted as FAT32
- Check Serial Monitor for "SD Card initialized" message
- Try different SD card
- Verify SD_CS pin (should be D4)

## Step 6: Button Testing

### Test Reset Functions

**Daily Reset (D6):**
1. Record current daily top speed and daily distance
2. Press and release D6 button
3. **Expected**: Both daily values reset to 0.0
4. Current speed and trip odometer unchanged

**Trip Reset (D7):**
1. Record current trip odometer
2. Press and release D7 button
3. **Expected**: Trip odometer resets to 0.0
4. Daily stats and current speed unchanged

**If buttons don't respond:**
- Check wiring (button should connect pin to GND)
- Verify button is momentary type (not latching)
- Try pressing and holding for 0.5 seconds
- Check Serial Monitor for "reset" messages

## Step 7: Real-World Testing

### Initial Cat Test

**Supervised First Run:**
1. Reset all values (both buttons)
2. Let cat use wheel normally
3. Watch display for:
   - Reasonable speed readings (most cats: 10-20 MPH sprinting)
   - Smooth speed updates (no wild jumps)
   - Correct distance accumulation

**Common Issues on First Cat Test:**

**Problem: Speed shows 0.0 even when cat is running**
- Sensor too far from magnets
- Magnets loose or falling off
- Wiring issue with Hall sensor

**Problem: Erratic speeds (jumps around wildly)**
- Magnets not evenly spaced
- Wheel wobbling
- Some magnets missing or loose

**Problem: Speed stuck at high value**
- Hall sensor stuck in LOW state
- Check sensor isn't touching magnets
- Verify sensor wiring

### Long-Term Monitoring

**Week 1: Data Collection**
- Monitor daily top speeds
- Compare to your expectations
- Note any unusual readings

**Week 2: Adjust if Needed**
- If speeds seem low: Check magnets, sensor position
- If speeds seem high: Adjust MAX_CAT_SPEED threshold
- If distance seems off: Recheck wheel measurements

### Performance Benchmarks

**Typical house cat performance:**
- **Walking/Trotting**: 3-8 MPH
- **Running**: 10-20 MPH
- **Sprinting**: 20-30 MPH (short bursts)
- **Daily distance**: 0.5-3 miles (very active cats)

**If you see:**
- Speeds consistently above 35 MPH: False readings, check validation
- Speeds below 3 MPH most of the time: Cat is walking (normal)
- Daily distance over 5 miles: Very energetic cat! (or sensor miscounting)

## Troubleshooting Calibration Issues

### Speed Too High
**Symptoms**: Always shows speeds above realistic values

**Causes & Fixes:**
1. **Wheel diameter too large in code**: Remeasure and adjust
2. **Missing magnets**: Count magnets, should be 12
3. **False triggers**: Increase MIN_MAGNET_INTERVAL
4. **Wrong magnet count in code**: Verify MAGNET_COUNT = 12

### Speed Too Low
**Symptoms**: Shows lower speeds than expected

**Causes & Fixes:**
1. **Wheel diameter too small in code**: Remeasure and adjust
2. **Extra magnets**: Count magnets, should be exactly 12
3. **Missing detections**: Move sensor closer to magnets

### Distance Incorrect
**Symptoms**: Odometer doesn't match actual rotations

**Causes & Fixes:**
1. **Diameter wrong**: Recalculate circumference
2. **Magnet count wrong**: Verify exactly 12 magnets
3. **Missed detections**: Check all magnets are detected
4. **Constant calculation error**: Verify INCHES_PER_MILE = 63360

### Inconsistent Readings
**Symptoms**: Speed jumps around, distance increments erratically

**Causes & Fixes:**
1. **Uneven magnet spacing**: Remount magnets evenly
2. **Wheel wobble**: Check wheel mechanical condition
3. **Sensor alignment**: Ensure sensor perpendicular to rim
4. **Loose magnets**: Secure all magnets firmly
5. **Electrical noise**: Add 0.1µF capacitor across Hall sensor VCC-GND

## Advanced Calibration

### Fine-Tuning Constants

For maximum accuracy, you can fine-tune these:

```cpp
// Wheel measurements (measure very carefully!)
const float WHEEL_DIAMETER = 118.0;  // Inches, outer diameter

// If you measured circumference directly instead:
// const float WHEEL_CIRCUMFERENCE = 370.7;  // Comment out diameter calc

// Magnet configuration
const int MAGNET_COUNT = 12;  // Must match actual magnets

// Timing (adjust based on testing)
const unsigned long SPEED_TIMEOUT = 4000;  // 4 seconds before speed → 0
const unsigned long MIN_MAGNET_INTERVAL = 45;  // Minimum valid magnet time

// Validation (adjust for your cats)
const float MAX_CAT_SPEED = 35.0;  // Maximum believable speed
```

### Statistical Validation

**Track data over time:**
1. Record daily top speeds for a week
2. Calculate average and maximum
3. If maximum is consistently near MAX_CAT_SPEED, increase limit
4. If readings are noisy, increase MIN_MAGNET_INTERVAL

### Professional Calibration

**Ultimate accuracy method:**
1. Use a laser tachometer to measure actual RPM
2. Compare to displayed speed
3. Calculate correction factor if needed
4. Apply to circumference or diameter constant

## Calibration Complete Checklist

Your system is calibrated when:

- [ ] All 12 magnets detected per rotation
- [ ] Speed readings realistic for cats (10-30 MPH typical)
- [ ] No false readings from wheel rocking
- [ ] Distance accurate (10 rotations ≈ 0.1 miles)
- [ ] Speed returns to 0.0 after stopping
- [ ] Daily stats persist through power loss
- [ ] Both reset buttons work correctly
- [ ] Display updates smoothly and correctly

**Congratulations!** Your cat wheel speedometer is ready for action! 🐱💨

## Maintenance

**Weekly:**
- Check magnets are secure
- Verify sensor alignment
- Clean any dust from sensor

**Monthly:**
- Backup SD card data
- Check all connections
- Verify calibration with manual rotation test

**As Needed:**
- Replace batteries (if using battery power)
- Re-secure loose magnets
- Recalibrate if wheel modified
