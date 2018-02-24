# Complete instructions for installing DirtyJTAG on a $2 chinese ST-Link clone (DFU method)

The DFU method needs fewer hardware (only an ST-Link dongle with its original firmware), but is a little bit more complicated to use : ST's bootloader take over the USB communication, so we need to send a USB packet to get out of the bootloader before using our dongle with UrJTAG.

## Uploading the firmware

Compile [stlink-tool](https://github.com/jeanthom/stlink-tool), and download the [DFU version](https://github.com/jeanthom/DirtyJTAG/releases) of the DirtyJTAG firmware.

Then run :

```
stlink-tool /path/to/dirtyjtag.stlinkv2dfu.bin
```

Once the flashing process is complete, unplug and plug the dongle.

## Getting out of the bootloader

Install [stlink](https://github.com/texane/stlink) and run the following command :

```
st-info --probe
```

The above command may return an error, that is expected behavior. Now you may check in `lsusb` if your DirtyJTAG probe is correctly recognized.

You'll need to launch this command every time you'll plug your ST-Link dongle in your computer. You may want to [automate it with a udev rule](https://lujji.github.io/blog/installing-blackmagic-via-stlink-bootloader/).
