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
#include <stdbool.h>
#include <string.h>
#include <unicore-mx/usbd/usbd.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/f1/bkp.h>
#include <unicore-mx/cm3/scb.h>

#include "jtag.h"
#include "usb.h"
#include "cmd.h"

enum CommandIdentifier {
  CMD_STOP = 0x00,
  CMD_INFO = 0x01,
  CMD_FREQ = 0x02,
  CMD_XFER = 0x03,
  CMD_SETSIG = 0x04,
  CMD_GETSIG = 0x05,
  CMD_CLK = 0x06,
  CMD_SETVOLTAGE = 0x07,
  CMD_GOTOBOOTLOADER = 0x08
};

enum CommandModifier {
  NO_READ = 0x80,
  EXTEND_LENGTH = 0x40
};

enum SignalIdentifier {
  SIG_TCK = 1 << 1,
  SIG_TDI = 1 << 2,
  SIG_TDO = 1 << 3,
  SIG_TMS = 1 << 4,
  SIG_TRST = 1 << 5,
  SIG_SRST = 1 << 6
};

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
static void cmd_xfer(usbd_device *usbd_dev, const uint8_t *commands, bool extend_length, bool no_read);

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
 * @brief Handle CMD_SETVOLTAGE command
 *
 * CMD_SETVOLTAGE sets the I/O voltage for devices that support this feature.
 *
 * @param commands Command data
 */
static void cmd_setvoltage(const uint8_t *commands);

/**
 * @brief Handle CMD_GOTOBOOTLOADER command
 *
 * CMD_GOTOBOOTLOADER resets the MCU and enters its bootloader (if installed)
 */
static void cmd_gotobootloader(void);

uint8_t cmd_handle(usbd_device *usbd_dev, const usbd_transfer *transfer) {
  uint8_t *commands = (uint8_t*)transfer->buffer;

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
    case CMD_XFER|NO_READ:
    case CMD_XFER|EXTEND_LENGTH:
    case CMD_XFER|NO_READ|EXTEND_LENGTH:
      cmd_xfer(usbd_dev, commands, *commands & EXTEND_LENGTH, *commands & NO_READ);
      return !!(*commands & NO_READ);
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

    case CMD_SETVOLTAGE:
      cmd_setvoltage(commands);
      commands += 1;
      break;

    case CMD_GOTOBOOTLOADER:
      cmd_gotobootloader();
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
  char info_string[64] = "DJTAG2\n";

  usb_send(usbd_dev, (uint8_t*)info_string, 64);
}

static void cmd_freq(const uint8_t *commands) {
  jtag_set_frequency((commands[1] << 8) | commands[2]);
}

static void cmd_xfer(usbd_device *usbd_dev, const uint8_t *commands, bool extend_length, bool no_read) {
  uint16_t transferred_bits;
  uint8_t output_buffer[64];
  
  /* Fill the output buffer with zeroes */
  if (!no_read) {
    memset(output_buffer, 0, sizeof(output_buffer));
  }

  /* This is the number of transfered bits in one transfer command */
  transferred_bits = commands[1];
  if (extend_length) {
    transferred_bits += 256;
  }
  
  jtag_transfer(transferred_bits, commands+2, output_buffer);

  /* Send the transfer response back to host */
  if (!no_read) {
    usb_send(usbd_dev, output_buffer, 64);
  }
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
  uint8_t signals, clk_pulses;

  signals = commands[1];
  clk_pulses = commands[2];

  jtag_strobe(clk_pulses, signals & SIG_TMS, signals & SIG_TDI);
}

static void cmd_setvoltage(const uint8_t *commands) {
  (void)commands;
}

static void cmd_gotobootloader(void) {
  // Magic RTC values from dapboot
  BKP_DR1 = 0x4F42;
  BKP_DR2 = 0x544F;
  scb_reset_system();
}
