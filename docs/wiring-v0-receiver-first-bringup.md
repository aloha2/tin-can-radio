# Tin-Can Radio V0 — Receiver First Wiring / Bring-up Plan

> Status note, 2026-06-24: this bring-up has now succeeded on the bench. The verified milestone is documented in `docs/milestone-v0-electronics-bringup.md`, and the next mechanical handoff is in `docs/hermes-handoff-v0-mechanical.md`.

This is the next practical step after the purchased-material inspection: build a table-top receiver test rig **outside the can first**, then move it into the can after the electronics are proven.

## Goal for this step

Get this chain working:

```text
Seeed XIAO ESP32C3
  → I2C control/display bus
  → RDA5807M FM receiver + 0.91" OLED
  → PAM8406 amplifier
  → 1× 4Ω 2-inch speaker
```

Do **not** install into the metal Coke can yet. First prove power, I2C, FM tuning, display, and audio on the bench.

---

## Important assumptions / cautions

1. The later `RDA5807M` pin-introduction image confirms the module pinout in **TOP View**. With the same orientation as the image, pin 1 is bottom-right:

```text
Top view, same orientation as RDA5807M pin image:

Left side:                  Right side:
Pin 6  DATA / SDA           Pin 5  FM antenna
Pin 7  CLOCK / SCL          Pin 4  RCK / NC
Pin 8  GP2 / NC             Pin 3  L-out
Pin 9  GP3 / NC             Pin 2  R-out
Pin 10 VDD +3.0/3.3 V       Pin 1  GND
```

Do not mirror this mapping unless you are viewing the module from the underside/back side.

2. The `0.91" OLED` pin order from the product image is:

```text
GND → VCC → SCL → SDA
```

3. XIAO ESP32C3 default I2C pins from Seeed documentation:

```text
D4 = SDA = GPIO6
D5 = SCL = GPIO7
```

4. The PAM8406 board photo shows:

```text
DC5V input:
  top terminal    = -
  bottom terminal = +

Speaker terminal order, top to bottom:
  R+
  R-
  L-
  L+
```

Because this is a Class-D / BTL amplifier, **do not connect speaker negative terminals to GND** and **do not tie L- and R- together**.

5. The small `5V充放电模块` is useful, but for first bring-up use a stable USB 5V source. Add the battery module only after the receiver/audio chain works.

---

## Recommended XIAO pin assignment

| Function | XIAO pin | ESP32-C3 GPIO | Notes |
|---|---:|---:|---|
| I2C SDA | `D4` | `GPIO6` | Shared by OLED + RDA5807M |
| I2C SCL | `D5` | `GPIO7` | Shared by OLED + RDA5807M |
| Rotary encoder A | `D0` | `GPIO2` | Use internal pull-up |
| Rotary encoder B | `D1` | `GPIO3` | Use internal pull-up |
| Encoder push / seek | `D2` | `GPIO4` | Use internal pull-up |
| Mode / mute / preset button | `D3` | `GPIO5` | Use internal pull-up |
| Optional extra button | `D6` | `GPIO21` | Leave free if you want UART debug |

Avoid using `D9 / GPIO9` for normal controls because it is related to boot mode on ESP32-C3 boards.

---

## Stage 1 — Power and I2C only

Wire only the XIAO, OLED, and RDA5807M control side.

### Common ground

```text
XIAO GND  → OLED GND
XIAO GND  → RDA5807M GND
```

### 3.3V power

```text
XIAO 3V3  → OLED VCC
XIAO 3V3  → RDA5807M VCC / 3V3
```

The RDA5807M IC is a low-voltage FM chip. Treat it as a `3.3V` device unless the exact module documentation says otherwise.

### I2C bus

```text
XIAO D4 / SDA  → OLED SDA
XIAO D5 / SCL  → OLED SCL
XIAO D4 / SDA  → RDA5807M SDA / SDIO
XIAO D5 / SCL  → RDA5807M SCL / SCLK
```

Expected common I2C addresses:

```text
OLED SSD1306: usually 0x3C, sometimes 0x3D
RDA5807M: commonly controlled at 0x10 / sequential register interface depending library
```

### Verification before audio

Run an I2C scanner sketch and confirm at least the OLED appears. Then run a tiny OLED test. Only after display works, add the RDA5807M library/test.

---

## Stage 2 — Controls

Wire the encoder and buttons to ground-switch inputs.

```text
Encoder common / C  → GND
Encoder A           → XIAO D0
Encoder B           → XIAO D1
Encoder SW          → XIAO D2
Mode button one leg → XIAO D3
Mode button other   → GND
```

In firmware, configure these pins as:

```cpp
INPUT_PULLUP
```

So the normal state is `HIGH`, pressed/closed state is `LOW`.

---

## Stage 3 — Audio path

### PAM8406 power

For first bench test:

```text
5V USB / bench 5V +  → PAM8406 DC5V + terminal
5V USB / bench GND   → PAM8406 DC5V - terminal
```

Also tie the logic/audio grounds together:

```text
XIAO GND / RDA GND → PAM8406 input GND / power GND
```

### RDA5807M audio to PAM8406

For one-speaker first test, use only one amplifier channel first:

```text
RDA5807M LOUT → PAM8406 L input
RDA5807M GND  → PAM8406 G input
```

Then connect the speaker to the left channel output pair only:

```text
Speaker + → PAM8406 L+
Speaker - → PAM8406 L-
```

Do **not** connect speaker negative to ground.

### Optional mono mix later

If you want both `LOUT` and `ROUT` mixed into one speaker channel, do not directly short them together. Use two resistors:

```text
RDA LOUT → 1k–4.7k resistor → mono audio node → PAM8406 L input
RDA ROUT → 1k–4.7k resistor → mono audio node → PAM8406 L input
RDA GND ------------------------------→ PAM8406 G input
```

---

## Stage 4 — Antenna

Current V0 bench antenna:

```text
0.35 mm² silver-plated OCC copper soft stranded wire, PTFE/Teflon jacket, 1 m
```

Electrical connection:

```text
RDA5807M ANT / FM_IN → one end of the 0.35 mm² soft wire antenna
Other end of antenna  → leave open / unconnected
```

Breadboard/mechanical connection, because the antenna is a soft stranded wire:

```text
Best: antenna wire → short soldered pigtail → solid breadboard jumper pin → breadboard row → RDA5807M ANT
Okay: antenna wire → Dupont female crimp/contact → male header pin on breadboard → RDA5807M ANT
Temporary only: stripped antenna end twisted tightly around a solid jumper pin, then taped/heat-shrunk
```

Do not try to push the soft strands directly into the breadboard hole; they will splay, make intermittent contact, and may leave broken strands behind. Tin the stripped end lightly before soldering/crimping, but avoid making a long stiff needle that can snap at the insulation edge. Add heat-shrink or tape as strain relief.

For bench testing, start with the full `1 m` wire stretched out or hanging away from the circuit. If it is inconvenient, keep about `75–80 cm` active/outside and route the extra length loosely; avoid a tight coil near the ESP32-C3, USB cable, amplifier, or inside the metal can.

If using the SMA/telescopic antenna instead:

```text
RDA5807M ANT / FM_IN → SMA center conductor / telescopic antenna center
Antenna shield / SMA ground → circuit GND, if the module/cable exposes it
```

Keep any antenna away from the ESP32-C3 Wi-Fi/BLE antenna and USB cable when testing FM sensitivity.

---

## Stage 5 — Battery power module, only after bench test works

Power module likely pads:

```text
18650 holder + → B+
18650 holder - → B-
Power module +5V → PAM8406 +5V and XIAO 5V/VBUS input
Power module GND / B- → system GND
```

Before connecting the XIAO and amp, verify with a multimeter:

```text
B+ to B- battery voltage: about 3.0–4.2V
+5V to GND output: about 5.0V
polarity is correct
```

The product image suggests the module may only reliably support a few hundred mA to around `1A` with voltage sag. Test amplifier loudness carefully.

---

## Bench bring-up order

1. XIAO powers up from USB.
2. OLED displays a test message.
3. I2C scanner sees OLED and/or RDA5807M.
4. RDA5807M initializes and tunes to a known local FM frequency.
5. Audio appears on one PAM8406 channel at low volume.
6. Encoder changes frequency or volume in firmware.
7. Add antenna and test reception quality.
8. Only then add 18650 power module.
9. Only after battery power is stable, start mechanical can layout.

---

## Stop conditions

Stop and debug before proceeding if any of these happen:

- XIAO gets hot.
- PAM8406 gets hot at low volume.
- 5V rail drops below about `4.7V` during normal audio.
- OLED flickers or resets when audio plays.
- Speaker negative accidentally touches GND or the metal can.
- RDA5807M is powered from 5V without confirmed 5V tolerance.

---

## Practical next physical action

Solder headers or short silicone wires to:

```text
OLED:      GND, VCC, SCL, SDA
RDA5807M:  VCC, GND, SDA, SCL, LOUT, GND, ANT
XIAO:      3V3, GND, D4, D5, D0, D1, D2, D3
```

Then build the I2C-only test first. Do not wire the amplifier until the display/RDA control side is confirmed.
