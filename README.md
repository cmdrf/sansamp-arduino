# sansamp-arduino
Arduino replacement for the digital innards of the original SansAmp PSA-1 guitar pre-amplifier.

The ROM in my SansAmp got corrupted, so I built this contraption from an Arduino Pro Mini and an OLED display. It plugs into the PLCC socket, replacing the MC68HC11 processor.

![Top View](img/sansamp1.jpg)

![OLED Front View](img/sansamp2.jpg)

The OLED mod is reversible, as the original 7-segment display is just bent down (the glue broke on my unit anyway).

## TODO

- MIDI support
- Proper PCB
