# Building DirtyJTAG

## Manual build

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
make bin PLATFORM=bluepill
```

Currently there are two platforms :

 * bluepill : Default build setting
 * stlinkv2 : Chinese ST-Link clone with specific pinout

Once the build is completed, your freshly compiled firmware will be available in `src/` as a binary file.

## Docker build

If you have [Docker](https://www.docker.com/) installed on your computer, you can run the `./build.sh` script that will automatically build DirtyJTAG and copy the firmware files out of the container.
