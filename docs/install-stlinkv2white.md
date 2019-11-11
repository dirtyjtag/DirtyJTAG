# Complete instructions for installing DirtyJTAG on a chinese ST-Link clone (white pod version)

In order to follow this tutorial, you will need an SWD programmer, like a $2 ST-Link dongle.

Those clones are a rough replica of what ST Microelectronics sells as "ST-LINK/V2" through many electronic component sellers.

## Buying an ST-Link clone

These boards can be bought for around 6€ (s&h incl.) on chinese marketplaces.

## Getting SWD on the ST-Link

To get started you will need to take apart your ST-Link pod. There are [two screws](https://youtu.be/zXENCKrPQMc?t=106) hidden beneath the label. Now you have to solder a 2x6 0.1" male hear to the unpopulated 2x6 connector. This is the connector you will use to program DirtyJTAG. The pinout should be written on the silkscreen.

Plug your SWD adapter on the freshly installed SWD header and install [stlink](https://github.com/texane/stlink). Once everything is set up you can use this command to flash DirtyJTAG on your ST-Link:

```
st-flash write /path/to/dirtyjtag.stlinkv2white.bin 0x8000000
```

## Pinout


