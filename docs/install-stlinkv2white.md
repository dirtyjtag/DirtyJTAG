# Complete instructions for installing DirtyJTAG on a chinese ST-Link clone (white pod version)

Those clones are a rough replica of what ST Microelectronics sells as "ST-LINK/V2" through many electronic component sellers. Unlike the official ST-Link v2, the chinese clones do not have features like I/O buffering with user settable voltage.

## Buy an ST-Link clone

These boards are widely available on internet marketplaces (AliExpress, Amazon, taobao, etc.) for around 6€.

## Prerequisites

 * A second ST-Link programmer
 * Spare .1" 2x3 header
 * [OpenOCD](http://openocd.org)
 * [stlink](https://github.com/texane/stlink)

## Installing SWD headers on the ST-Link

As the ST-Link is shipped flashed there is no need for the end user for an SWD header (in a normal use case). To install one you have to take apart the ST-Link pod by unscrewing its back (there are [two screws](https://youtu.be/zXENCKrPQMc?t=106) hidden under the label) and solder a 2x3 0.1" header to the PCD.

The pinout should be indicated on the PCB's silkscreen.

## Remove write protection

Any ST-Link currently sold (even the clones) have their readout protection enabled. This means that when you want to flash another firmware than the factory-supplied one, you will have to disable the read protection using OpenOCD. Hopefully you will only have to do it once!

Connect the ST-Link that will act as a programmer to the target on its SWD header. Open up a terminal and type in the following command:

```
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg
```

Now while keeping this terminal open, open another one and connect to the OpenOCD server:

```
telnet localhost 4444
```

Then type in the following commands to reset the readout protection and exit the telnet session:

```
halt
stm32f1x unlock 0
reset
exit
```

You can `CTRL+C` on the OpenOCD terminal to terminate it.

## Flashing DirtyJTAG

With your target ST-Link connected to the programmer ST-Link you can now flash DirtyJTAG:

```
st-flash write /path/to/dirtyjtag.stlinkv2white.bin 0x8000000
```

## Pinout


![Pinout of the ST-Link](img/stlinkv2white-pinout.jpg)
