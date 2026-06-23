# Tin-Can Radio V0 — VS Code Handoff for First Firmware Test

- Created: `2026-06-24 05:42:01 CST`
- Project folder: `C:\Users\wqy94\Desktop\makerProjects\tin-can radio`
- WSL path: `/mnt/c/Users/wqy94/Desktop/makerProjects/tin-can radio`
- Current build state: first breadboard wiring is finished; next step is firmware bring-up in VS Code.

## 1. Purpose of this handoff

This document is the practical coding handoff for moving from physical wiring to the first firmware test.

The first firmware goal is **not** a polished radio UI. The goal is to prove the electronics chain in small stages:

```text
Seeed XIAO ESP32C3
  → I2C bus
  → 0.91" OLED display
  → RDA5807M FM receiver
  → PAM8406 amplifier
  → 4Ω 2-inch speaker
  → 0.35 mm² × 1 m soft wire antenna
```

Recommended bring-up order:

```text
1. Board boots and Serial Monitor works.
2. OLED displays text.
3. I2C scanner detects OLED and/or RDA5807M.
4. RDA5807M initializes and tunes to one known local FM station.
5. Audio comes out through PAM8406 + speaker.
6. Add encoder/buttons only after basic radio works.
```

Related project docs:

```text
docs/wiring-v0-receiver-first-bringup.md
docs/tin-can-radio-materials-purchased.md
MATERIAL_LIST_V0.md
PROJECT_BRIEF.md
```

## 2. Recommended VS Code setup

Use VS Code with either **PlatformIO** or **Arduino IDE extension**. For this project, PlatformIO is the cleaner path because it keeps board config, libraries, and firmware in the project folder.

Recommended:

```text
VS Code + PlatformIO extension
```

Board target:

```text
Seeed XIAO ESP32C3
ESP32-C3, 3.3V logic
```

If PlatformIO does not show an exact `Seeed XIAO ESP32C3` board option, use an ESP32-C3 dev board target first and then adjust pins manually.

Possible PlatformIO board IDs to try:

```ini
board = seeed_xiao_esp32c3
```

If unavailable in your PlatformIO install, search the PlatformIO board list for:

```text
xiao esp32c3
esp32-c3
seeed
```

## 3. Suggested firmware folder structure

Create a firmware folder inside the project root:

```text
C:\Users\wqy94\Desktop\makerProjects\tin-can radio\firmware\tin-can-radio-v0
```

Equivalent WSL path:

```text
/mnt/c/Users/wqy94/Desktop/makerProjects/tin-can radio/firmware/tin-can-radio-v0
```

Suggested structure:

```text
firmware/tin-can-radio-v0/
  platformio.ini
  src/
    main.cpp
  README.md
```

Do not put firmware directly inside `docs/`; keep docs and source separate.

## 4. Known hardware wiring map

### 4.1 RDA5807M module pinout assumption

The current wiring plan assumes the RDA5807M pin-introduction image is viewed from the **top side**.

```text
Top view, same orientation as RDA5807M pin image:

Left side:                  Right side:
Pin 6  DATA / SDA           Pin 5  FM antenna
Pin 7  CLOCK / SCL          Pin 4  RCK / NC
Pin 8  GP2 / NC             Pin 3  L-out
Pin 9  GP3 / NC             Pin 2  R-out
Pin 10 VDD +3.0/3.3 V       Pin 1  GND
```

Do not mirror this mapping unless viewing the module from the underside/back side.

### 4.2 XIAO ESP32C3 I2C pins

Use the Seeed default I2C pins:

```text
XIAO D4 / GPIO6 → SDA
XIAO D5 / GPIO7 → SCL
```

Shared I2C bus:

```text
XIAO D4 / SDA → OLED SDA
XIAO D5 / SCL → OLED SCL
XIAO D4 / SDA → RDA5807M DATA / SDA / SDIO
XIAO D5 / SCL → RDA5807M CLOCK / SCL / SCLK
```

### 4.3 Power and ground

For first test, power logic from XIAO USB and keep the receiver/display at `3.3V`:

```text
XIAO GND → OLED GND
XIAO GND → RDA5807M GND
XIAO 3V3 → OLED VCC
XIAO 3V3 → RDA5807M VDD / VCC / 3V3
```

The amplifier is separate `5V`:

```text
5V USB / bench 5V + → PAM8406 DC5V +
5V USB / bench GND  → PAM8406 DC5V -
XIAO/RDA GND         → PAM8406 input GND / power GND
```

Important:

```text
All signal grounds must be common.
Speaker negative must NOT go to GND.
PAM8406 is Class-D / BTL; use L+ and L- as a pair.
```

### 4.4 Audio path for first mono test

Use only the left channel first:

```text
RDA5807M LOUT → PAM8406 L input
RDA5807M GND  → PAM8406 G input
Speaker +     → PAM8406 L+
Speaker -     → PAM8406 L-
```

Do not short `LOUT` and `ROUT` together. If mono mix is desired later:

```text
RDA LOUT → 1k–4.7k resistor → mono node → PAM8406 L input
RDA ROUT → 1k–4.7k resistor → mono node → PAM8406 L input
RDA GND ------------------------------→ PAM8406 G input
```

### 4.5 Antenna connection

Current antenna:

```text
0.35 mm² single-crystal/OCC copper, silver-plated, PTFE/Teflon jacket, soft stranded wire, 1 m
```

Connection:

```text
RDA5807M ANT / FM_IN / Pin 5 → one end of antenna wire
Other end of antenna          → open / unconnected
```

Because it is soft stranded wire, do not insert strands directly into breadboard holes. Use a solid adapter:

```text
Best:
antenna wire → solder joint → solid male jumper/header pin → breadboard row → RDA5807M ANT

Temporary:
antenna wire → tightly wrapped around solid jumper pin → tape/heat-shrink → breadboard row → RDA5807M ANT
```

Place the antenna away from:

```text
ESP32-C3 Wi-Fi/BLE antenna
USB cable
PAM8406 board
speaker wires
metal Coke can body
```

For first testing, use the full `1 m` wire stretched out or hanging freely. If reception is poor or routing is awkward, try about `75–80 cm` active length and route the extra loosely.

## 5. Pin assignment for later controls

Controls are not required for the first radio proof. Add later after OLED + RDA + audio work.

Planned controls:

```text
Encoder common / C  → GND
Encoder A           → XIAO D0 / GPIO2
Encoder B           → XIAO D1 / GPIO3
Encoder SW          → XIAO D2 / GPIO4
Mode button one leg → XIAO D3 / GPIO5
Mode button other   → GND
```

Firmware input mode:

```cpp
pinMode(pin, INPUT_PULLUP);
```

Meaning:

```text
Normal state: HIGH
Pressed/closed state: LOW
```

Avoid using:

```text
D9 / GPIO9
```

Reason: it is related to ESP32-C3 boot mode and can cause confusing boot issues.

## 6. PlatformIO starter config

Create `platformio.ini` like this:

```ini
[env:seeed_xiao_esp32c3]
platform = espressif32
board = seeed_xiao_esp32c3
framework = arduino
monitor_speed = 115200
upload_speed = 921600

lib_deps =
  adafruit/Adafruit SSD1306
  adafruit/Adafruit GFX Library
  mathertel/Radio
```

Notes:

- `Adafruit SSD1306` is for the `0.91"` OLED.
- `mathertel/Radio` commonly includes RDA5807M support, but library APIs may vary by version.
- If the RDA library does not compile, switch to another RDA5807M library or write minimal I2C register code.

## 7. First firmware test plan

### Stage A — Serial boot test

Create the absolute minimum firmware first:

```cpp
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Tin-Can Radio V0 boot OK");
}

void loop() {
  Serial.println("alive");
  delay(1000);
}
```

Expected result in VS Code Serial Monitor:

```text
Tin-Can Radio V0 boot OK
alive
alive
alive
```

If this does not work, do not debug OLED/radio yet. Fix board selection, USB cable, driver, boot mode, or serial port first.

### Stage B — I2C scanner

Use this to confirm I2C wiring:

```cpp
#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 6
#define I2C_SCL 7

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("I2C scanner start");
}

void loop() {
  byte error, address;
  int count = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.print("Devices found: ");
    Serial.println(count);
  }

  delay(3000);
}
```

Expected likely addresses:

```text
OLED SSD1306: usually 0x3C, sometimes 0x3D
RDA5807M: may appear at 0x10, 0x11, or may depend on library/register mode
```

If OLED appears but RDA does not, do not panic immediately. Some radio modules/libraries can behave differently in I2C scan than normal devices. Still check wiring and power.

### Stage C — OLED display test

Use OLED address `0x3C` first. If blank, try `0x3D`.

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C_SDA 6
#define I2C_SCL 7
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 init failed");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Tin-Can Radio V0");
  display.println("OLED OK");
  display.display();

  Serial.println("OLED OK");
}

void loop() {
}
```

Expected:

```text
OLED shows Tin-Can Radio V0 / OLED OK
Serial prints OLED OK
```

### Stage D — RDA5807M radio test

Pick one strong local FM station frequency for first test. Replace the placeholder before compiling:

```cpp
const float TEST_FREQ_MHZ =  FM_STATION_HERE;
```

Use a known strong station in your area. Examples of format:

```cpp
const float TEST_FREQ_MHZ =  101.7;
```

Library APIs vary, so treat this section as the intended behavior, not guaranteed final code:

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C_SDA 6
#define I2C_SCL 7
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const float TEST_FREQ_MHZ = 101.7; // replace with known strong local station

void showStatus(const char* line1, const char* line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  display.println(line2);
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(I2C_SDA, I2C_SCL);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  showStatus("Tin-Can Radio", "Init RDA5807M...");

  // TODO: initialize RDA5807M with chosen library
  // TODO: set band FM 87.5-108 MHz
  // TODO: set volume low first
  // TODO: tune TEST_FREQ_MHZ

  Serial.println("RDA5807M init/tune placeholder");
  showStatus("FM test", "Tune station");
}

void loop() {
  // TODO: optionally print RSSI/stereo status if library supports it
  delay(1000);
}
```

When the RDA library is selected, the real code should do:

```text
radio.init()
radio.setBand(FM broadcast band)
radio.setFrequency(TEST_FREQ_MHZ)
radio.setVolume(low-to-medium)
optional: read RSSI / stereo / station status
```

## 8. First test checklist before uploading radio code

Before applying power to the full chain:

```text
[ ] RDA5807M VDD is connected to 3.3V, not 5V.
[ ] OLED VCC is connected to 3.3V.
[ ] SDA/SCL are not swapped.
[ ] XIAO GND, RDA GND, OLED GND, and PAM8406 input/power GND are common.
[ ] PAM8406 speaker uses L+ and L-, not GND.
[ ] Antenna wire connects only to RDA ANT/FM_IN at one end; other end floats.
[ ] Antenna wire uses a solid jumper/header adapter, not loose strands in breadboard.
[ ] Volume is low before first audio test.
[ ] No exposed wire strands are touching adjacent breadboard rows.
[ ] Boards are outside the metal can for this first test.
```

## 9. Debugging guide

### Board does not upload

Check:

```text
USB cable supports data, not charge-only.
Correct serial port selected.
Correct ESP32-C3/XIAO board selected.
Hold BOOT while plugging in if needed.
Do not attach controls to boot-sensitive pins.
```

### Serial Monitor is garbage

Check:

```text
monitor_speed = 115200
Serial.begin(115200)
```

### I2C scanner finds nothing

Check:

```text
SDA/SCL swapped?
OLED/RDA powered?
Common GND connected?
Using GPIO6/GPIO7 for XIAO D4/D5?
Breadboard row mismatch?
```

### OLED blank but scanner finds `0x3C`

Try:

```text
Confirm screen height is 32, not 64.
Try OLED address 0x3D.
Check OLED pin order: GND → VCC → SCL → SDA.
```

### Radio initializes but no sound

Check:

```text
Antenna attached to ANT/FM_IN.
Tune a strong known local station.
PAM8406 has 5V power.
PAM8406 input GND tied to RDA/XIAO GND.
RDA LOUT goes to PAM8406 L input.
Speaker is on L+ and L-, not GND.
Volume is not muted / too low.
Try ROUT instead of LOUT in case module/channel labeling differs.
```

### Loud hum/noise

Check:

```text
USB cable and ESP32-C3 Wi-Fi antenna are too close to FM antenna.
Antenna tightly coiled near digital electronics.
PAM8406 input wire too long/unshielded.
Ground missing between audio source and amp.
Power source noisy.
```

### Reception weak

Try:

```text
Move antenna away from breadboard and USB cable.
Hang antenna vertically.
Try full 1 m length.
Try 75–80 cm active length.
Move near a window.
Disconnect from laptop charger and run laptop on battery.
Keep circuit outside the metal can.
```

## 10. Minimal milestone definition

The first firmware milestone is complete when:

```text
[ ] Serial boot message appears.
[ ] OLED shows project/status text.
[ ] I2C scanner confirms at least OLED; ideally also RDA5807M behavior.
[ ] RDA5807M tunes to a known local station.
[ ] Speaker outputs recognizable FM audio.
[ ] Antenna connection is mechanically stable enough to move gently without audio cutting out.
```

After this milestone, move to:

```text
1. Add encoder and buttons.
2. Add station seek/preset UI.
3. Add OLED frequency/status screen.
4. Package electronics into a safe insulated sled before putting anything into the can.
```

## 11. Important design note

Do not install into the Coke can yet. The can is conductive and has sharp edges after cutting. Finish the bench proof first, then design insulation, strain relief, antenna exit, and board mounting.

For the antenna exit in the final can build, plan:

```text
small hole + rubber grommet / printed bushing + strain relief
```

This prevents the can edge from cutting the PTFE wire jacket and prevents antenna movement from stressing the RDA5807M breadboard/module connection.
