# DirtyJTAG

[![Build Status](https://travis-ci.org/jeanthom/DirtyJTAG.svg?branch=master)](https://travis-ci.org/jeanthom/DirtyJTAG)

DirtyJTAG is a JTAG adapter firmware for $2 ST-Link clones and generic STM32 development boards ("blue pill"/"black pill" STM32F101 and STM32F103 based ARM boards). The DirtyJTAG project was created to find an alternative to the obsolete (but cheap) LPT Wiggler cables, and expensive USB JTAG probes.

DirtyJTAG is dirty and dirt cheap, but is not fast nor a perfect implementation of the JTAG protocol. Yet it is around 500 sloccount lines, therefore it is easily understandable and hackable.

DirtyJTAG is supported in mainline UrJTAG, see the ["Installing UrJTAG with DirtyJTAG support"](docs/urjtag-dirtyjtag.md) page.

If you prefer OpenOCD to UrJTAG, I suggest using Zoobab's fork of Versaloon firmware, which is available [on his GitHub repository](https://github.com/zoobab/versaloon).

## Build your DirtyJTAG adapter

| Bluepill                                    | ST-Link v2 clone (dongle)                                                               | ST-Link v2 clone (white case)                 | "Baite" ST-Link v2 clone                |
|:-------------------------------------------:|:---------------------------------------------------------------------------------------:|:---------------------------------------------:|:---------------------------------------:|
| ![Bluepill icon](docs/img/bluepill-128.jpg) | ![ST-Link v2 dongle](docs/img/stlinkv2-128.jpg)                                         | ![ST-Link v2](docs/img/stlink-white-128.jpg)  | ![Baite dongle](docs/img/baite-128.jpg) |
| [Instructions](docs/install-bluepill.md)    | [SWD (recommended)](docs/install-stlinkv2-swd.md) • [DFU](docs/install-stlinkv2-dfu.md) | [Instructions](docs/install-stlinkv2white.md) | No instructions yet                     |

## USB VID and PID

All DirtyJTAG-flashed devices appear with the following USB VID=0x1209/PID=0xC0CA
(which belongs to "InterBiometrics") :

```
$ lsusb
[...]
Bus 002 Device 003: ID 1209:c0ca InterBiometrics
```

The PID was obtained through http://pid.codes, which is a registry of USB
PID codes for open source hardware projects. More infos: http://pid.codes/1209/C0CA/

Dmesg also give some output where you can see the Manufacturer field as "DirtyJTAG":

```
$ dmesg
[  148.223008] usb 2-1: USB disconnect, device number 2
[  149.669794] usb 2-1: new full-speed USB device number 3 using xhci_hcd
[  149.812853] usb 2-1: New USB device found, idVendor=1209, idProduct=c0ca
[  149.812859] usb 2-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[  149.812863] usb 2-1: Product: Jean THOMAS
[  149.812866] usb 2-1: Manufacturer: DirtyJTAG
[  149.812869] usb 2-1: SerialNumber: no serial :-)
```

## Some docs

 * [Compiling DirtyJTAG yourself](docs/building-dirtyjtag.md)
 * [Compiling UrJTAG with DirtyJTAG support](docs/urjtag-dirtyjtag.md)
 * [Validate your DirtyJTAG cable with JTAG targets](docs/jtag-validation.md)

## Inspiration & links

 * [opendous-jtag](https://github.com/vfonov/opendous-jtag)
 * [neroJtag](https://github.com/makestuff/neroJtag)
 * [clujtag-avr](https://github.com/ClusterM/clujtag-avr)
 * [CuVoodoo page on JTAG and ST-Link v2 adapters](https://wiki.cuvoodoo.info/doku.php?id=jtag)
 * [versaloon jtag](https://github.com/zoobab/versaloon)
 * [blackmagic probe on st-link-v2 clones](https://madnessinthedarkness.transsys.com/blog:2017:0122_black_magic_probe_bmp_on_st-link_v2_clones)
 * [ST-Link clone repurposing](https://hackaday.io/project/162597-st-link-clone-repurposing)
 * [ST-Link clone as STM32 dev board](https://blog.danman.eu/st-link-clone-as-stm32-dev-board/)
 * [Repurpose an ST-Link clone](https://www.hobbiton.be/blog/repurpose-stlink/)
