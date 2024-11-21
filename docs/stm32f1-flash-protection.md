# Fixing STM32F1xx flash protection issues

One of the most common issues when working with pre-programmed STM32F1 MCUs is having the flash protection bit set. You might encounter messages like these when the protection is activated:

```
(..)
2021-01-28T20:34:11 INFO flash_loader.c: Successfully loaded flash loader in sram
2021-01-28T20:34:14 ERROR flash_loader.c: flash loader run error
2021-01-28T20:34:14 ERROR common.c: stlink_flash_loader_run(0x8000000) failed! == -1
stlink_fwrite_flash() == -1
```

The following documentation explains how to disable it through two different techniques. **Be aware that disabling flash protection will erase the whole flash!**

## Unlocking method #1: OpenOCD

This first method requires you to connect an ST-Link programmer to your target. If you're unsure how to do so, check out [our documentation](https://github.com/dirtyjtag/DirtyJTAG/tree/master/docs).

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

## Unlocking method #2: stm32flash

[@aalku](https://github.com/aalku) offers another method for removing the flash lock by using [stm32flash](https://github.com/stm32duino/stm32flash). stm32flash is a small utility that lets you flash STM32 devices using the built-in UART bootloader. Hardware-wise you will only need a cheap USB-UART dongle. The following two commands disable the read-protection and write-protection:

```
stm32flash -k /dev/ttyUSB0
stm32flash -u /dev/ttyUSB0
```

## Working with STM32 clones

[@brainstorm](https://github.com/brainstorm) reported that the CKS32 clones can also be unlocked using the above procedure. However OpenOCD does not support CKS32 out of the box so you need to patch the STM32 configuration file to include the JTAG id for CKS32:

```diff
--- stm32f1x.cfg	2021-01-28 22:55:43.000000000 +1100
+++ ck32.cfg	2021-01-28 22:55:55.000000000 +1100
@@ -39,7 +39,7 @@
       set _CPUTAPID 0x3ba00477
    } {
       # this is the SW-DP tap id not the jtag tap id
-      set _CPUTAPID 0x1ba01477
+      set _CPUTAPID 0x2ba01477
    }
 }
```
