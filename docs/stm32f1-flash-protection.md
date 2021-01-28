# Fixing STM32F1xx flash protection issues

One of the most common issues when working with STM32F1 MCUs is having the flash protection bit set. The following documentation explains how to disable it through two different techniques.

**Be aware: disabling flash protection will erase the whole flash.**

## Unlocking method #1: OpenOCD

This first method requires you to connect an ST-Link programmer to your target. If you're unsure how to do so, check out [our documentation](https://github.com/jeanthom/DirtyJTAG/tree/master/docs).

This procedure requires you to open two terminals. In the first terminal, start OpenOCD:

```
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg
```

Then while keeping OpenOCD open, connect to OpenOCD in your other terminal:

```
telnet localhost 4444
```

You should now have an OpenOCD prompt (congratulations!). You may now enter the following sequence of commands to reset the MCU flash protection (and its content by the way).

```
halt
stm32f1x unlock 0
reset
exit
```

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
