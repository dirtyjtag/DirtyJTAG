#!/usr/bin/env python3
# escape-bootloader.py sends the right command to ST's bootloader to run the
# main application

import usb

# find our device
dev = usb.core.find(idVendor=0x0483, idProduct=0x3748)

# was it found?
if dev is None:
    raise ValueError('Device not found')

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration()

dev.write(2, [0xF3, 0x07])