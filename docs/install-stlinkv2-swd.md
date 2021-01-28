# Installing DirtyJTAG on a ST-Link clone dongle

ST-Link clone dongles are the cheap dongles you can find on AliExpress. They provide the minimum amount of signals needed for JTAG operation. They currently require an SWD programmer in order to install DirtyJTAG on it.

## Requirements

 * ST-Link v2 dongle
 * An ST Link v2-compatible SWD programmer
 * [stlink](https://github.com/texane/stlink) and OpenOCD installed on your computer

## SWD wiring

To gain access to the internal SWD connector in your ST-Link dongle, press firmly the USB connector againt a flat surface while holding the outer aluminium casing as shown:

![Tearing down an ST-Link programmer](img/stlinkv2-teardown.gif)

![Two ST-Link clones connected together](img/stlinkv2-programming.png)

If you don't want to solder to your ST-Link dongle, you can either use [dupont header cables](img/stlinkv2-header-cable.jpg) or like zoobab hack a small jig with pogo pins ([[1]](img/stlinkv2-jig-1.jpg), [[2]](img/stlinkv2-jig-2.jpg)).

## Remove flash protection

Any ST-Link currently sold (even the clones) have their readout protection enabled. This means that when you want to flash another firmware than the factory-supplied one, you will have to disable the read protection using OpenOCD. Hopefully you will only have to do it once!

Connect the ST-Link that will act as a programmer to the target on its SWD header. Open up a terminal and type in the following command:

```
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg
```

You might get an error similar to
```
Warn : UNEXPECTED idcode: 0x2ba01477 Error: expected 1 of 1: 0x1ba01477
```
which indicates that your ST-Link V2 clone has a STM32 clone MCU.

In this case, create a custom config file with the reported idcode, eg "`my-stlink-v2.cfg`" with the content:
```
source [find interface/stlink-v2.cfg]
set CPUTAPID 0x2ba01477
```
and substitute `-f interface/stlink-v2.cfg` with `-f my-stlink-v2.cfg` when invoking openocd.

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

More detailed explanations are available in [a separate documentation page](stm32f1-flash-protection.md).

## Flashing DirtyJTAG

Download a binary version of DirtyJTAG (make sure to get the `baite` build!) or [compile it yourself](building-dirtyjtag.md), then flash it using `st-flash`:

```
st-flash write /path/to/dirtyjtag.stlinkv2.bin 0x8000000
```

## Pinout

![ST-Link pinout with DirtyJTAG firmware](img/stlink-pinout.jpg)

A print friendly version [is also available](img/stlinkv2-sticker.svg).
