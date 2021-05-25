/*
  Copyright (c) 2017-2021 The DirtyJTAG authors.

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software
  is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <unicore-mx/usbd/usbd.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/rcc.h>


#include "delay.h"
#include "cmd.h"
#include "usb.h"

#define DIRTYJTAG_USB_BUFFER_SIZE 64

/**
 * @brief Registers the callback for incomming USB packet
 *
 * This function registers the callback which be called when
 * an incomming USB packet arrives. It must be called each time
 * you want to receive a USB packet.
 *
 * @param usbd_dev USB device
 */
static void usb_prepare_rx(usbd_device *usbd_dev);

/**
 * @brief USB RX callback
 *
 * This function will be called whenever a USB packet arrives.
 * It will process the received packet. It will automatically
 * call usb_prepare_rx() in order to receive new packets.
 *
 * @param usbd_dev USB device
 * @param transfer Packet
 * @param status Transfer status
 * @param usb_id URB id
 */
static void usb_rx_callback(usbd_device *usbd_dev, const usbd_transfer *transfer,
			    usbd_transfer_status status, usbd_urb_id urb_id);

/**
 * @brief USB TX callback
 *
 * This function will be called after a packet a correctly been
 * sent. It will automatically call usb_prepare_rx() in order to
 * receive new packets.
 *
 * @param usbd_dev USB device
 * @param transfer Packet
 * @param status Transfer status
 * @param urb_id URB id
 */
static void usb_tx_callback(usbd_device *usbd_dev, const usbd_transfer *transfer,
			    usbd_transfer_status status, usbd_urb_id urb_id);

typedef uint8_t usb_buffer[DIRTYJTAG_USB_BUFFER_SIZE];
usb_buffer tx_usb_buffer;
usb_buffer rx_usb_buffer[2];

static const struct usb_string_descriptor string_lang_list = {
	.bLength = USB_DT_STRING_SIZE(1),
	.bDescriptorType = USB_DT_STRING,
	.wData = {
		USB_LANGID_ENGLISH_UNITED_STATES
	}
};

/* Generated using unicore-mx/scripts/usb-strings.py */
static const struct usb_string_descriptor product_string = {
	.bLength = USB_DT_STRING_SIZE(9),
	.bDescriptorType = USB_DT_STRING,
	/* "DirtyJTAG" */
	.wData = {
		0x0044, 0x0069, 0x0072, 0x0074, 0x0079, 0x004a, 0x0054, 0x0041,
		0x0047
	}
};

static const struct usb_string_descriptor man_string = {
	.bLength = USB_DT_STRING_SIZE(11),
	.bDescriptorType = USB_DT_STRING,
	/* Jean THOMAS */
	.wData = {
		0x004a, 0x0065, 0x0061, 0x006e, 0x0020, 0x0054, 0x0048, 0x004f,
		0x004d, 0x0041, 0x0053
	}
};

static struct usb_string_descriptor serial_string = {
	.bLength = USB_DT_STRING_SIZE(24),
	.bDescriptorType = USB_DT_STRING,
	.wData = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
};

static const struct usb_string_descriptor **string_data[1] = {
  (const struct usb_string_descriptor *[]){&man_string, &product_string, &serial_string}
};

static const struct usbd_info_string string = {
	.lang_list = &string_lang_list,
	.count = 3,
	.data = string_data
};

const struct __attribute__((packed)) {
  struct usb_config_descriptor config;
  struct usb_interface_descriptor iface;
  struct usb_endpoint_descriptor endp[2];
} config_desc = {
  .config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = sizeof(config_desc),
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x32, /* 100mA current consumption */
  },

  .iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_VENDOR, /* Custom USB class */
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = 0,
  },

  .endp = {
    {
      .bLength = USB_DT_ENDPOINT_SIZE,
      .bDescriptorType = USB_DT_ENDPOINT,
      .bEndpointAddress = DIRTYJTAG_READ_ENDPOINT,
      .bmAttributes = USB_ENDPOINT_ATTR_BULK,
      .wMaxPacketSize = 64,
      .bInterval = 1
    }, {
      .bLength = USB_DT_ENDPOINT_SIZE,
      .bDescriptorType = USB_DT_ENDPOINT,
      .bEndpointAddress = DIRTYJTAG_WRITE_ENDPOINT,
      .bmAttributes = USB_ENDPOINT_ATTR_BULK,
      .wMaxPacketSize = 64,
      .bInterval = 1
    }
  }
};

const struct usb_device_descriptor dev_desc = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0110,
	.bDeviceClass = USB_CLASS_VENDOR,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x1209,
	.idProduct = 0xC0CA,
	.bcdDevice = 0x0110,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1
};

static const struct usbd_info info = {
	.device = {
		.desc = &dev_desc,
		.string = &string
	},

	.config = {{
		.desc = (const struct usb_config_descriptor *) &config_desc,
		.string = &string
	}}
};

static char hex_to_ascii(uint8_t c) {
  return (c < 10) ? c + '0' : c + 'A' - 10;
}

void usb_read_serial(void) {
  volatile uint8_t *uid;
  size_t i;

  uid = (volatile uint8_t *)0x1FFFF7E8;

  for (i = 0; i < 12; i++) {
    serial_string.wData[2*i] = hex_to_ascii((uid[i] & 0xF0) >> 4);
    serial_string.wData[2*i+1] = hex_to_ascii(uid[i] & 0xF);
  }
}

static void usb_set_config(usbd_device *usbd_dev,
			   const struct usb_config_descriptor *cfg) {
  (void)cfg;

  usbd_ep_prepare(usbd_dev, DIRTYJTAG_READ_ENDPOINT, USBD_EP_BULK, 64,
		  USBD_INTERVAL_NA, USBD_EP_NONE);
  usbd_ep_prepare(usbd_dev, DIRTYJTAG_WRITE_ENDPOINT, USBD_EP_BULK, 64,
		  USBD_INTERVAL_NA, USBD_EP_NONE);

  usb_prepare_rx(usbd_dev);
}

static void usb_control_request(usbd_device *usbd_dev, uint8_t ep,
				const struct usb_setup_data *setup_data) {
  (void) ep; /* assuming ep == 0 */

  const uint8_t bmReqMask = USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT;
  const uint8_t bmReqVal = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;

  if ((setup_data->bmRequestType & bmReqMask) != bmReqVal) {
    /* Pass on to usb stack internal */
    usbd_ep0_setup(usbd_dev, setup_data);
    return;
  }

  usbd_ep0_stall(usbd_dev);
}

void usb_init(void) {
  usbd_device *usbd_dev;

  usb_read_serial();

  /* USB device initialisation */
  usbd_dev = usbd_init(USBD_STM32_FSDEV, NULL, &info);
  usbd_register_set_config_callback(usbd_dev, usb_set_config);
  usbd_register_setup_callback(usbd_dev, usb_control_request);

  while (1) {
    usbd_poll(usbd_dev, 0);
  }
}

void usb_reenumerate(void) {
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_OPENDRAIN, GPIO12);
  gpio_clear(GPIOA, GPIO12);

  delay_us(20000);
}

void usb_send(usbd_device *usbd_dev, uint8_t *sent_buffer, uint8_t sent_size) {
  const usbd_transfer transfer = {
    .ep_type = USBD_EP_BULK,
    .ep_addr = DIRTYJTAG_WRITE_ENDPOINT,
    .ep_size = DIRTYJTAG_USB_BUFFER_SIZE,
    .ep_interval = USBD_INTERVAL_NA,
    .buffer = tx_usb_buffer,
    .length = sent_size,
    .flags = USBD_FLAG_SHORT_PACKET,
    .timeout = USBD_TIMEOUT_NEVER,
    .callback = usb_tx_callback
  };

  /* Packets are copied into a buffer because the operation
     is done asynchronously */
  memcpy(tx_usb_buffer, sent_buffer, sent_size);

  usbd_transfer_submit(usbd_dev, &transfer);
}

static uint8_t submit_buffer_index = 0;

static void usb_prepare_rx(usbd_device *usbd_dev) {
  const usbd_transfer transfer = {
    .ep_type = USBD_EP_BULK,
    .ep_addr = DIRTYJTAG_READ_ENDPOINT,
    .ep_size = DIRTYJTAG_USB_BUFFER_SIZE,
    .ep_interval = USBD_INTERVAL_NA,
    .buffer = rx_usb_buffer[submit_buffer_index],
    .length = DIRTYJTAG_USB_BUFFER_SIZE,
    .flags = USBD_FLAG_SHORT_PACKET,
    .timeout = USBD_TIMEOUT_NEVER,
    .callback = usb_rx_callback
  };

  usbd_transfer_submit(usbd_dev, &transfer);
  submit_buffer_index ^= 1;
}

static void usb_rx_callback(usbd_device *usbd_dev, const usbd_transfer *transfer,
			    usbd_transfer_status status, usbd_urb_id urb_id) {
  (void)urb_id;

  if (status == USBD_SUCCESS) {
    if (transfer->transferred) {
      usb_prepare_rx(usbd_dev);
      cmd_handle(usbd_dev, transfer);
    } else {
      usbd_transfer_submit(usbd_dev, transfer); /* re-submit */
    }
  }
}

static void usb_tx_callback(usbd_device *usbd_dev, const usbd_transfer *transfer,
			    usbd_transfer_status status, usbd_urb_id urb_id) {
  (void)urb_id;
  (void)transfer;
  (void)usbd_dev;
  (void)status;
}
