# Small FM/Bluetooth Tin-Can Radio — Project Brief

## Working concept

A compact tabletop FM/Bluetooth radio built directly inside a real, original-label tin can.

The project should feel like a found-object DIY radio, not a plastic product pretending to be a can.

## Core design principle

```text
The can is not decoration.
The can is the enclosure, chassis, acoustic body, visual identity, and main design constraint.
```

## Chosen enclosure direction

```text
Option A — Original label preserved
```

Use real tin cans with their original food/product labels kept visible whenever possible. The label becomes part of the product identity. Each unit may be slightly different depending on the can used.

## Product character

- Raw tin-can DIY object
- Found-object / ready-made feeling
- Visible handmade construction, but intentionally designed
- Small, charming, tactile, and functional
- A mix of tin-can telephone, camp radio, garage electronics, and beach tabletop speaker

## Target first prototype

A real tin can that:

- plays Bluetooth audio;
- plays FM radio;
- has physical controls;
- can be powered or charged by USB-C;
- has a small internal speaker;
- does not rattle badly;
- looks intentionally designed rather than randomly hacked together.

## Recommended V0 feature scope

```text
V0 = FM + Bluetooth + single speaker + rechargeable battery + physical volume + mode/power switch
```

Avoid for V0:

```text
Wi-Fi
smart assistant
screen-heavy interface
stereo system
recording
mobile app control
full custom PCB
```

## Likely physical architecture

```text
Front / open end:
- speaker grille or speaker baffle
- 3D printed ring fitted into the can rim
- visible screws/washers if useful

Side wall:
- volume knob
- mode/power switch
- USB-C charging port
- optional status LED hole

Top:
- antenna, wire loop, or small handle detail

Inside:
- Bluetooth/FM audio module
- amplifier module if needed
- speaker driver
- rechargeable battery
- charging/protection module
- 3D printed internal sled/spine
- foam/rubber anti-rattle details
```

## Mechanical strategy

### Speaker mounting

Use a custom front baffle/ring rather than loose glue.

The front ring should:

- hold the speaker securely;
- fit the tin-can rim;
- reduce rattling;
- provide screw positions;
- visually finish the cut/open edge.

### Electronics mounting

Use a removable internal sled/spine.

The sled can hold:

- battery;
- PCB modules;
- switch;
- USB-C board;
- wiring strain relief.

### Anti-rattle strategy

Use:

- thin EVA foam;
- rubber washers;
- felt or acoustic damping;
- nylon/M2/M3 standoffs;
- hot glue only as secondary support.

Do not over-dampen the can. A small amount of metallic character may be part of the object’s charm.

## Visual language

Preferred details:

- original product label preserved;
- visible screws and washers;
- knurled metal knob;
- small toggle or slide switch;
- black rubber feet;
- simple punched/perforated speaker grille;
- telescopic antenna or wire antenna;
- minimal added graphics, so the found label remains dominant.

## Open decisions

Before CAD and layout:

1. Exact can format: soup/tomato can, coffee/tea tin, sardine tin, paint can, etc.
2. Speaker diameter and depth.
3. Whether FM tuning is handled by buttons, encoder, or module auto-scan.
4. Battery format: `18650` cell vs flat LiPo pack.
5. Whether to expose a telescopic antenna or hide a wire antenna.
6. Whether the front face uses a visible speaker driver, punched metal grille, or fabric/mesh insert.

## Immediate next step

Purchase/shortlist the core BOM before choosing the exact can format, so the can can be selected around real component sizes and mounting needs.
