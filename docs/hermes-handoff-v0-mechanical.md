# Hermes Handoff: Tin-Can Radio V0 Mechanical Integration

- Date: 2026-06-24
- Handoff target: later mechanical work with Hermes agent
- Current state: electronics and firmware work on the bench
- Mechanical goal: move the verified bench radio into a safe insulated can assembly without breaking the working electrical behavior

## Current verified state

The electronics are no longer speculative. The bench radio has received clear FM audio at `101.7 MHz`, the OLED works at `0x3C`, the RDA5807M responds on the I2C bus, and encoder tuning works with acceleration.

Power has also been tested:

```text
18650 cell -> 5V charge/boost module -> XIAO ESP32C3 + PAM8406
```

Measured values:

```text
5V module output before full load: about 5.1V
5V module output during full radio test: 5.06V
Thermal result: all items stayed cool
```

The separate 5V supply is important. Without it, turning on the amplifier caused the OLED to go out.

## Preserve this electrical architecture

Keep this power split:

```text
5V charge/boost module +5V -> XIAO 5V/VBUS
5V charge/boost module +5V -> PAM8406 DC5V+
XIAO 3V3                  -> OLED VCC
XIAO 3V3                  -> RDA5807M VDD/VCC
All grounds               -> common system GND
```

Do not power the OLED or RDA5807M from 5V.

Keep this I2C wiring:

```text
XIAO D4/GPIO6 -> OLED SDA and RDA5807M DATA/SDA
XIAO D5/GPIO7 -> OLED SCL and RDA5807M CLOCK/SCL
```

Keep this encoder wiring:

```text
Encoder A      -> XIAO D0/GPIO2
Encoder B      -> XIAO D1/GPIO3
Encoder SW     -> XIAO D2/GPIO4
Encoder common -> GND
```

Keep this audio rule:

```text
RDA5807M LOUT -> PAM8406 L input
Speaker +     -> PAM8406 L+
Speaker -     -> PAM8406 L-
```

The PAM8406 speaker output is bridge-tied load. Do not connect speaker negative to ground, and do not let any speaker terminal touch the metal can.

## Mechanical tasks

Design and build the can assembly around the already-working electrical layout.

Core mechanical tasks:

- Build a non-conductive internal sled or spine for the XIAO, RDA5807M, OLED, PAM8406, boost/charge module, and 18650 holder.
- Add fishpaper, Kapton, EVA foam, printed standoffs, or another non-conductive barrier anywhere a board, wire, or battery could touch the can.
- Make a front speaker baffle or ring for the 4 Ohm 2-inch speaker.
- Add speaker mounting that seals reasonably but does not crush the cone or frame.
- Place the OLED where it is visible without straining its wires.
- Place the encoder so the knob is usable and does not collide with the speaker, battery, or can wall.
- Keep the PAM8406 volume knob accessible with enough clearance for fingers.
- Keep the USB-C charging port accessible from outside the can.
- Retain the 18650 securely so it cannot slide, rotate into solder joints, or rattle.
- Add antenna exit with a rubber grommet or printed bushing.
- Add strain relief for the antenna so pulling the wire does not load the RDA5807M module.
- Add foam/rubber anti-rattle points after all electrical tests pass.

## Recommended Hermes workflow

Follow this sequence to avoid losing the known-good electrical state:

1. Photograph the current bench wiring from multiple angles before disassembly.
2. Make a paper or CAD layout of all modules before cutting the can.
3. Build the non-conductive sled outside the can.
4. Transfer electronics onto the sled and retest outside the can.
5. Only after the sled retest passes, cut the can and speaker opening.
6. Deburr every cut edge.
7. Add grommets or bushings before routing wires through metal.
8. Install the sled into the can without the battery first and check for shorts.
9. Add battery power and retest after each mechanical step.
10. Do final volume/reception testing with the can closed.

## No-go items

Do not proceed if any of these are true:

- Any PCB pad, solder joint, battery terminal, or bare wire can touch the metal can.
- The antenna passes through raw cut metal without a grommet or bushing.
- The speaker negative terminal is connected to GND.
- The speaker terminals can touch the can.
- The OLED or RDA5807M are powered from 5V.
- The battery can move enough to stress its wires.
- The charge/boost module is buried so deeply that USB-C charging access is unreliable.
- The amplifier or boost module gets warm during a short closed-can test.

## Antenna handling

The current V0 antenna is a soft stranded PTFE wire. It should not be trapped tightly inside the metal can.

Recommended:

```text
RDA5807M ANT -> insulated antenna wire -> grommeted can exit -> free outside length
```

Keep the antenna away from:

- XIAO ESP32C3 antenna area
- USB cable
- PAM8406 board
- speaker wires
- battery/boost converter wiring

Do not rely on a sharp hole in the can as the antenna exit. The metal edge can cut the jacket over time.

## Firmware reference

Use the working PlatformIO environment:

```text
stage6_encoder_radio
```

Build/upload command used during bring-up:

```powershell
C:\Users\wqy94\.platformio\penv\Scripts\python.exe -m platformio run -e stage6_encoder_radio -t upload --upload-port COM5
```

The COM port can change. If upload fails, check Device Manager or PlatformIO ports.

Important current firmware settings:

```text
Startup station: 101.7 MHz
RDA volume: 6
Slow tune step: 0.1 MHz
Fast tune step: 0.5 MHz
Very fast tune step: 1.0 MHz
```

## Optional future electrical work

These are not required for mechanical integration:

- Battery percentage display using a resistor divider to `D3/GPIO5/A3`.
- Better battery calibration curve for the 18650.
- FM preset storage.
- Seek mode.
- Better RDA5807M library/status handling.
- Bluetooth input using a separate Bluetooth audio receiver module.
- FM/Bluetooth input switch or resistor mixer before the PAM8406.

Do not add these during first mechanical packaging unless the basic radio still works after every change.

## Acceptance criteria for the mechanical phase

The mechanical phase is successful when:

- The radio still boots from the 18650 and 5V module.
- OLED remains on when the amplifier is enabled.
- Encoder tuning still changes frequency.
- `101.7 MHz` is still recognizable.
- No module gets hot.
- Nothing rattles enough to stress wiring.
- No live conductor can contact the can.
- Battery can be charged without opening the can.
- Antenna exits through an insulated, strain-relieved path.
