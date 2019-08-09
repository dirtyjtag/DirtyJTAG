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

#include <stdint.h>
#include <string.h>
#include <unicore-mx/usbd/usbd.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/flash.h>

#include "boot-bypass.h"
#include "jtag.h"
#include "usb.h"
#include "cmd.h"

#define CMD_STOP 0x00
#define CMD_INFO 0x01
#define CMD_FREQ 0x02
#define CMD_XFER 0x03
#define CMD_SETSIG 0x04
#define CMD_GETSIG 0x05
#define CMD_CLK 0x06
#define CMD_BOOTLOADER_PATCH 0x07

#define SIG_TCK (1 << 1)
#define SIG_TDI (1 << 2)
#define SIG_TDO (1 << 3)
#define SIG_TMS (1 << 4)
#define SIG_TRST (1 << 5)
#define SIG_SRST (1 << 6)

#define FLASH_PAGE_SIZE_F103 0x400

/**
 * @brief Handle CMD_INFO command
 *
 * CMD_INFO returns a string to the host software. This
 * could be used to check DirtyJTAG firmware version
 * or supported commands. As of now it is implemented
 * but not usefull.
 *
 * @param usbd_dev USB device
 */
static void cmd_info(usbd_device *usbd_dev);

/**
 * @brief Handle CMD_FREQ command
 *
 * CMD_FREQ sets the clock frequency on the probe.
 * Currently this does not changes anything.
 *
 * @param commands Command data
 */
static void cmd_freq(const uint8_t *commands);

/**
 * @brief Handle CMD_XFER command
 *
 * CMD_XFER reads and writes data simultaneously.
 *
 * @param usbd_dev USB device
 * @param commands Command data
 */
static void cmd_xfer(usbd_device *usbd_dev, const uint8_t *commands);

/**
 * @brief Handle CMD_SETSIG command
 *
 * CMD_SETSIG set the logic state of the JTAG signals.
 *
 * @param commands Command data
 */
static void cmd_setsig(const uint8_t *commands);

/**
 * @brief Handle CMD_GETSIG command
 *
 * CMD_GETSIG gets the current signal state.
 * 
 * @param usbd_dev USB device
 */
static void cmd_getsig(usbd_device *usbd_dev);

/**
 * @brief Handle CMD_CLK command
 *
 * CMD_CLK sends clock pulses with specific TMS and TDI state.
 *
 * @param commands Command data
 */
static void cmd_clk(const uint8_t *commands);

/**
 * @brief Patch ST's bootloader to directly jump to DirtyJTAG firmware
 *
 * Patch the first instruction of the bootloader to directly jump to where
 * DirtyJTAG is. This command takes one byte as parameter:
 *  - 0x01 means "Kill the bootloader"
 *  - 0x00 means "Re-enable the bootloader"
 *
 * @param commands Command data
 */
static void cmd_bootloader_patch(const uint8_t *commands);

uint8_t cmd_handle(usbd_device *usbd_dev, const usbd_transfer *transfer) {
  uint8_t *commands;

  commands = (uint8_t*)transfer->buffer;
  
  while (*commands != CMD_STOP) {
    switch (*commands) {
    case CMD_INFO:
      cmd_info(usbd_dev);
      break;
      
    case CMD_FREQ:
      cmd_freq(commands);
      commands += 2;
      break;

    case CMD_XFER:
      cmd_xfer(usbd_dev, commands);
      return 0;
      break;

    case CMD_SETSIG:
      cmd_setsig(commands);
      commands += 2;
      break;

    case CMD_GETSIG:
      cmd_getsig(usbd_dev);
      return 0;
      break;

    case CMD_CLK:
      cmd_clk(commands);
      commands += 2;
      break;

    case CMD_BOOTLOADER_PATCH:
      cmd_bootloader_patch(commands);
      commands += 1;
      break;
      
    default:
      return 1; /* Unsupported command, halt */
      break;
    }

    commands++;
  }

  return 1;
}

static void cmd_info(usbd_device *usbd_dev) {
  char info_string[64] = "DJTAG1\n";

  usb_send(usbd_dev, (uint8_t*)info_string, 64);
}

static void cmd_freq(const uint8_t *commands) {
  jtag_set_frequency((commands[1] << 8) | commands[2]);
}

static void cmd_xfer(usbd_device *usbd_dev, const uint8_t *commands) {
  uint8_t transferred_bits;
  uint8_t output_buffer[32];
  
  /* Fill the output buffer with zeroes */
  memset(output_buffer, 0, 32);

  /* This is the number of transfered bits in one transfer command */
  transferred_bits = commands[1];
  
  jtag_transfer(transferred_bits, commands+2, output_buffer);

  /* Send the transfer response back to host */
  usb_send(usbd_dev, output_buffer, 32);
}

static void cmd_setsig(const uint8_t *commands) {
  uint8_t signal_mask, signal_status;

  signal_mask = commands[1];
  signal_status = commands[2];

  if (signal_mask & SIG_TCK) {
    jtag_set_tck(signal_status & SIG_TCK);
  }

  if (signal_mask & SIG_TDI) {
    jtag_set_tdi(signal_status & SIG_TDI);
  }

  if (signal_mask & SIG_TMS) {
    jtag_set_tms(signal_status & SIG_TMS);
  }
  
  if (signal_mask & SIG_TRST) {
    jtag_set_trst(signal_status & SIG_TRST);
  }

  if (signal_mask & SIG_SRST) {
    jtag_set_srst(signal_status & SIG_SRST);
  }
}

static void cmd_getsig(usbd_device *usbd_dev) {
  uint8_t signal_status = 0;
  
  if (jtag_get_tdo()) {
    signal_status |= SIG_TDO;
  }

  usb_send(usbd_dev, &signal_status, 1);
}

static void cmd_clk(const uint8_t *commands) {
  uint8_t signals, clk_pulses, i;

  signals = commands[1];
  clk_pulses = commands[2];

  /* Set TDI & TMS signals */
  jtag_set_tms(signals & SIG_TMS);
  jtag_set_tdi(signals & SIG_TDI);

  /* Send clock pulses */
  for (i = 0; i < clk_pulses; i++) {
    jtag_clock();
  }
}

static void cmd_bootloader_patch(const uint8_t *commands) {
  void *const start_addr = (void*)0x08000000;
  uint8_t page_buf[FLASH_PAGE_SIZE_F103];
  size_t i;

  /* Backup flash page */
  memcpy(page_buf, start_addr, sizeof(page_buf));

  if (commands[1] == 0x01) {
    /* Todo; check CRC */

    /* Copy patch */
    memcpy(page_buf, _binary_src_boot_bypass_bin_start, _binary_src_boot_bypass_bin_size);

    /* Unlock flash */
    flash_unlock();

    /* Erase flash page */
    flash_erase_page((uint32_t)start_addr);
    if(flash_get_status_flags() != FLASH_SR_EOP)
      return;

    for (i = 0; i < FLASH_PAGE_SIZE_F103; i += 4) {
      flash_program_word((uint32_t)start_addr+i, *(uint32_t*)(page_buf+i));
    }
  }
}
