# Building DirtyJTAG

## Manual build

In order to compile DirtyJTAG, you will need the following software :

 * git
 * ARM toolchain (I'm using Fedora's `arm-none-eabi-gcc-cs`/`arm-none-eabi-newline` packages)
 * make

Clone this repository :

```
git clone --recursive https://github.com/jeanthom/dirtyjtag
cd dirtyjtag
```

Then you can build the firmware :

```
make PLATFORM=bluepill
```

Currently there are two platforms :

 * bluepill : Default build setting
 * stlinkv2 : Chinese ST-Link clone with specific pinout (the one that looks like a USB key)
 * stlinkv2dfu : Same as above but can be loaded by the bootloader (unsupported, known to be buggy)
 * olimexstm32h103 : STM32F103 board from Olimex
 * baite : "Baite" ST-Link dongle 

Once the build is completed, your freshly compiled firmware will be available in `src/` as a binary file.

## Docker build

If you have [Docker](https://www.docker.com/) installed on your computer, you can run the `./build.sh` script that will automatically build DirtyJTAG and copy the firmware files out of the container.
