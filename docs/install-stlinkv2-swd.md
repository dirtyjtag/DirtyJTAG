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

## Reflashing TI MSP432P401R builtin XDS 110 JTAG programmer firmware using DirtyJtag+openocd

To reflash the TI MSP432P401R builtin XDS 110 JTAG programmer firmware using DirtyJtag+openocd, follow these steps:

1. Connect the DirtyJTAG adapter to the TI MSP432P401R device using the appropriate JTAG connections.
2. Create an OpenOCD configuration file for the TI MSP432P401R device. You can use the following example as a starting point:

```
#
# TI MSP432P401R Chip
#
# Author : 
# Version : 
#
source [find interface/dirtyjtag.cfg]
# transport select jtag

set WORKAREASIZE 0x8000
set CHIPNAME msp432p401r
source [find target/stellaris.cfg]
```

3. Launch OpenOCD with the configuration file:

```
openocd -f /path/to/your/config/file.cfg
```

4. In a separate terminal, connect to the OpenOCD server using telnet:

```
telnet localhost 4444
```

5. Use the following commands to reflash the firmware:

```
halt
flash write_image erase /path/to/your/firmware.bin 0x00000000
reset
exit
```

## Troubleshooting

### Error: 103 214 bitq.c:308 bitq_execute_queue(): missing data from bitq interface

This error might be due to a lack of response from the TI chip, a bug in the openocd driver, or the DirtyJTAG firmware. To troubleshoot this issue, you can try the following steps:

1. Ensure that the JTAG connections are secure and correctly wired.
2. Use a Logic Analyzer to spy on the JTAG interface and see if the exchange right before the error message makes sense.
3. Try using an older version of the DirtyJTAG support for OpenOCD, which can be found [here](https://sourceforge.net/u/phdussud/openocd/).
4. Verify that you are using the correct OpenOCD configuration file for the TI MSP432P401R device.
5. Check for any updates or patches to the DirtyJTAG firmware or OpenOCD that might address this issue.
