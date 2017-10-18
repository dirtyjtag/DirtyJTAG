# DirtyJTAG

DirtyJTAG is a JTAG adapter firmware for $2 STM32 development boards ("blue pill"/"black pill" STM32F103-based ARM boards). The DirtyJTAG project was created to find an alternative to the obsolete (but cheap) LPT Wiggler cables, and expensive USB JTAG probes.

DirtyJTAG is dirty and dirt cheap, but is not fast nor a perfect implementation of the JTAG protocol. Yet it is a bit less than 500 sloccount lines, therefore it is easily understandable and hackable.

DirtyJTAG is currently only supported by urjtag through custom patches (I hope getting mainline support some time soon though).

If you prefer OpenOCD to UrJTAG, I suggest using Zoobab's fork of Versaloon firmware, which is available [on his GitHub repository](https://github.com/zoobab/versaloon).

## How to flash DirtyJTAG on a "Blue Pill" board

Download a pre-built version of the firmware (available [on the release page](https://github.com/jeanthomas/dirtyjtag/releases)) or build the firmware yourself (instructions provided below). Install [stlink](https://github.com/texane/stlink), then use this command :

```
st-flash write /path/to/dirtyjtag.bin 0x8000000
```

## Pinout

| STM32 | JTAG |
|-------|------|
| PA0   | TDI  |
| PA1   | TDO  |
| PA2   | TCK  |
| PA3   | TMS  |
| PA4   | TRST |
| PA5   | SRST |

Pin definition can be modified in `src/jtag.c`.

## How to patch urjtag for DirtyJTAG support

See [urjtag4dirtyjtag](https://github.com/jeanthom/urjtag4dirtyjtag) which is a patched version of urjtag with DirtyJTAG support. I'll try to push those changes upstream some time soon.

## DirtyJTAG compilation

In order to compile DirtyJTAG, you will need the following software :

 * git
 * ARM toolchain (I'm using Fedora's `arm-none-eabi-gcc-cs`/`arm-none-eabi-newline` packages)
 * make

Clone this repository :

```
git clone https://github.com/jeanthom/dirtyjtag
cd dirtyjtag
```

Then download unicore-mx :

```
git submodule init
git submodule update
```

Then you can build the firmware :

```
make bin
```

Your freshly compiled firmware will appear as `src/dirtyjtag.bin`.

## Inspiration

 * [opendous-jtag](https://github.com/vfonov/opendous-jtag)
 * [neroJtag](https://github.com/makestuff/neroJtag)
 * [clujtag-avr](https://github.com/ClusterM/clujtag-avr)
