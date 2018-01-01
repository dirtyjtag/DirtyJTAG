# DirtyJTAG

DirtyJTAG is a JTAG adapter firmware for $2 ST-Link clones and generic STM32 development boards ("blue pill"/"black pill" STM32F101 and STM32F103 based ARM boards). The DirtyJTAG project was created to find an alternative to the obsolete (but cheap) LPT Wiggler cables, and expensive USB JTAG probes.

DirtyJTAG is dirty and dirt cheap, but is not fast nor a perfect implementation of the JTAG protocol. Yet it is around 500 sloccount lines, therefore it is easily understandable and hackable.

DirtyJTAG is supported in mainline UrJTAG, see the ["Installing UrJTAG with DirtyJTAG support"](docs/urjtag-dirtyjtag.md) page.

If you prefer OpenOCD to UrJTAG, I suggest using Zoobab's fork of Versaloon firmware, which is available [on his GitHub repository](https://github.com/zoobab/versaloon).

## Build your DirtyJTAG adapter

 * [Instructions for $2 St-Link v2 clones](docs/install-stlinkv2.md)
 * [Instructions for $2 Blue Pill development board](docs/install-bluepill.md)

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
