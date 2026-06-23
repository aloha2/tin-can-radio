# Tin-Can Radio — Material List V0

## Current enclosure direction

- Real original-label beverage can, especially Coca-Cola or beer can for first demo.
- Typical `330 ml` beverage can dimensions from packaging references: about `66 mm` diameter × `115 mm` height.
- This strongly favors `2 inch / 50 mm` speaker drivers and small modules.

## Recommended V0 architecture

```text
Bluetooth/FM source board
→ small 5V amplifier
→ 1× 2 inch full-range speaker
→ 1× 18650 USB-C charge/boost power module
```

For a first demo, prioritize working audio + clean physical integration over perfect hi-fi sound.

---

## 1. Bluetooth + FM source board

### Preferred V0 board type

Buy a small generic board in this family:

```text
5V Bluetooth MP3 FM decoder board
Bluetooth 5.0
FM 87.5–108 MHz
USB / TF / AUX optional
line-level L/R/GND output
MODE / PLAY / VOL+ / VOL- buttons
external antenna wire/pad
```

Useful Taobao search terms:

```text
5V 蓝牙5.0 FM 收音 解码板 USB TF AUX
蓝牙 MP3 解码板 FM 收音 5V 线路输出
AVN-757 蓝牙 FM 解码板 5V
JQ-D013BT 蓝牙 FM 解码板
```

### Model-family notes

Marketplace names are inconsistent. The same-looking boards may be sold as:

```text
AVN-757 style board
JQ-D013BT style board
Bluetooth MP3 decoder board with FM
Car Bluetooth MP3 FM decoder board
```

Check listing photos for:

- separate `L / R / GND` audio output cable/pads;
- `5V` or `3.7–5V` supply support;
- FM mode explicitly listed;
- antenna pad/wire, usually marked `ANT` or included as a wire;
- not `12V only`.

### Avoid for this project

Avoid:

```text
large dashboard panel boards
12V-only car boards
boards that only output to headphones but do not expose solder pads
boards with giant mandatory display panels
remote-control-only operation
```

### Design recommendation

For the raw tin-can object, hide this module inside the can and expose only:

```text
MODE button
NEXT / PREV button if needed
maybe a tiny status LED hole
```

Do not let a car-MP3 front panel dominate the can aesthetic.

---

## 2. Amplifier board

### Preferred V0 amp

```text
PAM8403 5V 2×3W Class-D amplifier board
with volume potentiometer, preferably with knob/switch
```

Useful search terms:

```text
PAM8403 功放板 带音量旋钮 5V 2x3W
PAM8403 音频功放板 电位器 开关 5V
```

### Does it have to be PAM8403?

No. But `PAM8403` is the safest first choice because:

- it runs directly from `5V`;
- it is tiny;
- it is cheap;
- it is efficient and low-heat;
- it matches `3W–5W` small speakers;
- many boards include a ready-to-mount physical volume potentiometer.

### Alternative amplifier ICs/modules

| Option | Use if | Notes |
|---|---|---|
| `PAM8403` | first demo, 5V, 2 inch speaker | best default |
| `NS8002` mono 3W | smallest mono build | simple, but less common as nice modules |
| `PAM8406` | slightly more headroom | often `5V–12V`, check board details |
| `TPA3110` | larger/louder can build | usually wants higher voltage, bigger board |
| `TPA3116` | serious speaker project | too large/overpowered for Coke-can V0 |

### Is the volume knob necessary?

Not electrically mandatory, but highly recommended for this project.

Reasons:

- the knob gives the object tactile product value;
- Bluetooth phone volume alone feels less like a radio;
- FM mode benefits from physical volume;
- a potentiometer board reduces extra wiring.

Possible approaches:

| Volume method | Recommendation |
|---|---|
| Phone/module digital volume only | acceptable for hidden test rig, weak as product interaction |
| PAM8403 board with potentiometer | best V0 choice |
| Separate panel potentiometer wired to amp input | best later design control, more wiring |
| Rotary encoder controlling MCU | future custom version, not V0 |

---

## 3. Speaker options

## Mono option — recommended for one Coke/beer can

### Best branded 2 inch option

```text
Visaton FRS 5 X - 8 Ohm
```

Known specs from manufacturer/supplier data:

```text
Nominal size: 2 inch / 5 cm
Impedance: 8Ω
Rated power: 5W
Frequency response: about 120–20000 Hz
Sensitivity: about 86 dB
Cutout: about 46 mm
Depth: about 33 mm
Mounting: 2 lugs
```

Why good:

- physically fits `66 mm` beverage cans better than bigger drivers;
- reputable brand with datasheet;
- 8Ω load is easy on a tiny amp;
- mounting lugs can screw to a 3D printed baffle.

Tradeoff:

- with `PAM8403`, 8Ω will be less loud than 4Ω, but likely cleaner and safer.

### Better/louder branded 2 inch option

```text
Tang Band / TB Speakers W2-800SL
```

Known specs from supplier data:

```text
Nominal size: 2 inch
Impedance: 4Ω
Rated power: 10W
Frequency response: about 160–20000 Hz
Sensitivity: about 87 dB
Cutout: about 52 mm
Depth: about 30 mm
Mounting: 4 holes
```

Why good:

- `4Ω` gets more output from a `5V` class-D amp;
- strong small full-range driver;
- shallow enough for small cans.

Tradeoff:

- cutout is larger, leaving less baffle margin in a `66 mm` can.

### Budget Taobao test option

```text
2寸 全频喇叭 4欧 3W
```

Buy a pair as sacrificial test parts. Useful for cutting/mounting experiments before installing a nicer driver.

## Stereo / polyphonic option

For a single Coke/beer can, true stereo is physically awkward because the two speakers are too close together. Better options:

### Option S1 — two-can stereo pair

```text
Left can: 1× 2 inch driver
Right can: 1× 2 inch driver
Shared source/amp in one can or split electronics
```

Recommended drivers:

```text
2× Visaton FRS 5 X - 8Ω
```

or:

```text
2× Tang Band W2-800SL - 4Ω
```

This is the best portfolio direction: two original-label cans as a stereo pair.

### Option S2 — one can, dual micro-speaker

Use:

```text
2× 1 inch to 1.5 inch micro full-range speakers
```

Search:

```text
1.5寸 全频喇叭 4欧 3W
```

This can fit one can, but sound and stereo image will be worse than one good `2 inch` driver.

### Option S3 — mono speaker + passive radiator

Use:

```text
1× 2 inch active driver
1× small passive radiator
```

This is not stereo, but may sound fuller. Harder mechanically in a thin beverage can.

### Recommendation

For V0:

```text
Mono: 1× Visaton FRS 5 X - 8Ω or 1× Tang Band W2-800SL - 4Ω
```

For V1:

```text
Stereo pair: 2 cans, each with one 2 inch full-range driver
```

---

## 4. FM antenna

FM broadcast band is `87.5–108 MHz`. A quarter-wave wire around the middle of the band is roughly:

```text
~75 cm at 100 MHz
~69–85 cm across the FM band
```

### Best V0 antenna

```text
70–80 cm flexible insulated wire
```

### Current purchased antenna wire

```text
0.35 mm² silver-plated OCC copper soft stranded wire, PTFE/Teflon jacket, 1 m
```

Use this as the V0 bench antenna. Start with the full `1 m` length; if reception is unstable or you want closer FM quarter-wave behavior, leave only about `75–80 cm` outside the radio and coil or route the remaining slack loosely. Do not tightly coil the whole antenna inside the metal can.

Search:

```text
FM 收音机 天线 软线
FM 天线 75cm 软线
```

Why:

- cheap;
- easiest to solder to `ANT` pad;
- can be hidden, coiled loosely, or allowed to trail out of the can;
- less mechanical risk than a telescopic antenna.

### Best visual antenna

```text
small telescopic FM antenna, collapsed length around 10–15 cm, extended length 50–75 cm
```

Search:

```text
收音机 拉杆天线 小型 7节
FM 收音机 伸缩天线 带螺母
```

Look for:

- panel-mount threaded base with nut;
- collapsed length short enough for the can;
- extended length near `60–75 cm`;
- solder lug or wire tail;
- not a Wi-Fi/GSM antenna.

### Avoid

Avoid Taobao listings primarily for:

```text
2.4G WiFi 天线
GSM/4G 天线
GPS 天线
TV DTMB 天线
```

They may physically connect, but they are not tuned/practical for FM radio aesthetics or performance.

### Mechanical recommendation for Coke/beer can

For V0, use both:

```text
1× 0.35 mm² × 1 m soft wire antenna for reliable first test
1× small telescopic antenna for visual/product experiment
```

The metal can body will affect reception. Keep the active antenna wire/telescopic part outside the can, and isolate the antenna feed from sharp metal edges with a grommet.

---

## 5. Beverage can implications

For Coke/beer cans around `330 ml`:

```text
Diameter: about 66 mm
Height: about 115 mm
```

Design implications:

- `2 inch / 50 mm` speaker is the practical maximum for the front.
- `66 mm` outside diameter leaves limited screw/baffle margin.
- Thin aluminum will cut easily but dent and rattle easily.
- Use a 3D printed inner sleeve/baffle to avoid relying on the thin can wall for structure.
- Preserve label by cutting openings carefully and choosing control placement around important graphics.

Recommended can layout:

```text
front end: 2 inch speaker + printed baffle ring
side: volume knob + tiny mode button
rear/side lower: USB-C charging port
upper/rear: antenna exit
inside: vertical sled holding battery + boards
```

---

## 6. Dremel implications

A Dremel expands the project possibilities:

- cleaner speaker opening;
- small slots for USB-C and switches;
- deburring sharp aluminum edges;
- perforated grille experiments;
- polishing/riveted details;
- shaping a 3D printed baffle after test fitting.

Recommended model choice:

```text
Dremel 4250 = best default bench tool
Dremel 8240 = choose if cordless/no cable matters
```

For this project, prioritize accessories:

```text
flex shaft
reinforced cutting wheels
sanding drums
small burrs/grinding stones
small drill bits/collets
safety glasses
dust mask
clamps
```

The flex shaft is especially useful for thin beverage cans because it gives more delicate control than holding the full rotary tool body.

Safety notes:

- Beverage-can aluminum is sharp after cutting.
- Deburr every edge.
- Use eye protection; cutoff wheels can shatter.
- Practice on empty scrap cans before cutting the real label can.

---

## Recommended purchase bundle

### Minimum working mono V0

```text
1× AVN-757/JQ-D013BT-style 5V Bluetooth/FM decoder board
1× PAM8403 5V 2×3W amp board with potentiometer
1× Visaton FRS 5 X - 8Ω or Tang Band W2-800SL - 4Ω
1× IP5306-style USB-C 18650 charge/boost module
1× protected 18650 cell
1× 18650 holder
1× 75 cm FM antenna wire
1× small telescopic FM antenna
1× M2/M3 screw and standoff kit
1× EVA foam/felt/rubber washer kit
```

### Extra experimental parts

```text
2× budget 2 inch 4Ω 3W full-range speakers
2× 1.5 inch 4Ω 3W micro speakers
extra tactile buttons
extra mini toggle switches
USB-C breakout/extension board
JST connector kit
26AWG silicone wire
heat-shrink tube
```
