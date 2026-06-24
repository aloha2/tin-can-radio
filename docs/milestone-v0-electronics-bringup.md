# Tin-Can Radio V0 Electronics Bring-Up Milestone

- Date: 2026-06-24
- Project: `C:\Users\wqy94\Desktop\makerProjects\tin-can radio`
- Firmware project: `Tin-Can Radio`
- Milestone status: bench electronics and firmware proven

## What is now proven

The V0 radio electronics are working on the bench:

```text
18650 cell
  -> 5V charge/boost module
  -> XIAO ESP32C3 + PAM8406 amplifier

XIAO ESP32C3 3V3
  -> SSD1306 OLED + RDA5807M FM receiver

RDA5807M audio out
  -> PAM8406 amplifier
  -> 4 Ohm 2-inch speaker
```

The system receives recognizable FM audio, displays status on the OLED, and tunes by rotary encoder.

## Verified observations

- OLED I2C address: `0x3C`
- RDA5807M I2C behavior observed at: `0x10` and `0x11`
- Clear local station found: `101.7 MHz`
- Working firmware environment: `stage6_encoder_radio`
- Current RDA volume build flag: `RDA_VOLUME=6`
- 5V module open-circuit output measured by user: about `5.1V`
- Full system powered from the 5V module measured by user: `5.06V`
- Modules stayed cool during the full-power test.
- Adding the separate 5V supply fixed the OLED/display brownout when the amplifier knob was turned on.
- Encoder tuning works and acceleration feels acceptable for now.

## Current firmware behavior

Use this PlatformIO environment for the working radio:

```text
stage6_encoder_radio
```

Current important build settings:

```text
TEST_FREQ_MHZ=101.7
RDA_VOLUME=6
TUNE_STEP_MHZ=0.1
TUNE_FAST_STEP_MHZ=0.5
TUNE_VERY_FAST_STEP_MHZ=1.0
TUNE_FAST_THRESHOLD_MS=180
TUNE_VERY_FAST_THRESHOLD_MS=80
ENCODER_REVERSE=0
```

Runtime behavior:

- Boots at `101.7 MHz`.
- OLED shows radio status.
- Slow encoder rotation tunes in `0.1 MHz` steps.
- Faster encoder rotation tunes in `0.5 MHz` or `1.0 MHz` steps.
- Encoder press returns to the startup station.

Upload command used during bring-up:

```powershell
C:\Users\wqy94\.platformio\penv\Scripts\python.exe -m platformio run -e stage6_encoder_radio -t upload --upload-port COM5
```

`COM5` was correct during this milestone, but the port can change after reconnecting USB.

## Current wiring summary

Logic and receiver/display:

```text
XIAO 3V3       -> OLED VCC
XIAO 3V3       -> RDA5807M VDD/VCC
XIAO GND       -> OLED GND, RDA GND, PAM8406 GND, 5V module GND
XIAO D4/GPIO6  -> OLED SDA, RDA DATA/SDA
XIAO D5/GPIO7  -> OLED SCL, RDA CLOCK/SCL
```

Controls:

```text
Encoder A      -> XIAO D0/GPIO2
Encoder B      -> XIAO D1/GPIO3
Encoder SW     -> XIAO D2/GPIO4
Encoder common -> GND
```

Power:

```text
18650 +        -> 5V charge/boost module B+
18650 -        -> 5V charge/boost module B-
5V module +5V  -> XIAO 5V/VBUS and PAM8406 DC5V+
5V module GND  -> system GND and PAM8406 DC5V-
```

Audio:

```text
RDA5807M LOUT  -> PAM8406 L input
RDA5807M GND   -> PAM8406 input G/GND
Speaker +      -> PAM8406 L+
Speaker -      -> PAM8406 L-
```

Important: the PAM8406 is a BTL amplifier. Speaker negative must stay on `L-` or `R-`; it must not connect to GND.

## Known limitations

- The XIAO ESP32C3 cannot act as an iPhone Bluetooth audio receiver for this build. It supports BLE, not Bluetooth Classic A2DP audio.
- The physical PAM8406 volume knob is analog and not readable by the MCU in the current wiring.
- Battery percentage is not implemented yet.
- A rough battery gauge can be added later with a resistor divider from battery B+ to `D3/GPIO5/A3`.
- Do not connect the 18650 voltage directly to an ADC pin. Use a divider, for example `100k` over `100k`.
- The boosted 5V rail is not a good battery-percentage signal because the boost module holds it near 5V until the battery is low.
- RDA status bits such as tuned/RSSI may be jumpy with the current minimal driver. The reliable validation for this milestone is real received audio plus OLED/control behavior.

## Stop conditions still apply

Stop and debug before continuing if:

- XIAO ESP32C3 gets hot.
- PAM8406 gets hot at low or moderate volume.
- 5V rail drops below about `4.7V`.
- OLED flickers or resets during audio playback.
- Speaker negative touches GND or the metal can.
- OLED or RDA5807M are accidentally powered from 5V.
- Battery, boost module, or wiring smells hot or changes shape.

## Mechanical-build warning

Do not install this directly into the metal can yet. The can is conductive and cut edges are sharp. The next phase should design:

- insulated internal sled/spine
- speaker baffle
- strain relief
- antenna exit
- USB-C charging access
- battery retention
- foam or rubber anti-rattle supports

Use `docs/hermes-handoff-v0-mechanical.md` as the handoff for that work.
