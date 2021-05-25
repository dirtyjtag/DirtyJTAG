#!/usr/bin/env python3

import usb

# find our device
dev = usb.core.find(idVendor=0x1209, idProduct=0xC0CA)

# was it found?
if dev is None:
    raise ValueError('Device not found')

try:
	# set the active configuration. With no arguments, the first
	# configuration will be the active one
	dev.set_configuration()

	# Send CMD_GOTOBOOTLOADER then CMD_STOP
	dev.write(1, [0x08, 0x00])
except Exception as e:
	print("goto-bootloader failed")
