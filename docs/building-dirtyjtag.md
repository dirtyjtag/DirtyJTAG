# Building DirtyJTAG

## Manual build

In order to compile DirtyJTAG, you will need the following software:

 * git
 * ARM toolchain (I'm using Fedora's `arm-none-eabi-gcc-cs`/`arm-none-eabi-newline` packages)
 * make

Clone this repository :

```
git clone --recursive https://github.com/jeanthom/dirtyjtag
cd dirtyjtag
```

Then you can build all the firmware versions :

```
make
```

Once the build is completed, your freshly compiled firmware will be available in `src/` as a binary file.

If you only want to build a specific configuration of DirtyJTAG, directly call the right Makefile with the correct parameters for the platform and the bootloader type. Before that, make sure you have a build of unicore-mx.

```
make -C unicore-mx all
make -f Makefile.stm32f1 PLATFORM=bluepill LOADER=noloader
```

`PLATFORM` could be any of the following:

 * `bluepill`
 * `stlinkv2`
 * `stlinkv2white`
 * `baite`
 * `olimexstm32h103`

`LOADER` could be any of the following:

 * `noloader`: No bootloader
 * `loader2k`: Bootloader is occupying 0x0-0x2000, DirtyJTAG starts at 0x2000 offset
 * `loader2k`: Bootloader is occupying 0x0-0x4000, DirtyJTAG starts at 0x4000 offset

## Docker build

If you have [Docker](https://www.docker.com/) installed on your computer, you can run the `./build.sh` script that will automatically build DirtyJTAG and copy the firmware files out of the container.
