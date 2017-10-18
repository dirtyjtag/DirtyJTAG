/*
  Copyright (c) 2017 Jean THOMAS.
  
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

#include <unicore-mx/usbd/usbd.h>

#define DIRTYJTAG_READ_ENDPOINT 0x01
#define DIRTYJTAG_WRITE_ENDPOINT 0x82

/**
 * @brief Initialises USB peripheral on the STM32F103
 */
void usb_init(void);

/**
 * @brief Forces USB reenumeration by pulling data lines
 */
void usb_reenumerate(void);

/**
 * @brief Send a bulk USB packet
 *
 * @param usbd_dev USB device
 * @param sent_buffer Data buffer
 * @param size Size in bytes
 */
void usb_send(usbd_device *usbd_dev, uint8_t *sent_buffer, uint8_t size);
