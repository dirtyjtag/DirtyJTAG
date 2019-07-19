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

/**
 * @brief Initialize JTAG
 * 
 * Set GPIO in correct state for JTAG operation
 */
void jtag_init(void);

/**
 * @brief Set JTAG clock frequency
 *
 * Set JTAG clock frequency. Currently not working.
 *
 * @param frequency Frequency in kHz
 */
void jtag_set_frequency(uint32_t frequency);

/**
 * @brief Send a JTAG clock pulse
 */
void jtag_clock(void);

/**
 * @brief Set TCK state
 *
 * @param value TCK logic level
 */
void jtag_set_tck(uint8_t value);

/**
 * @brief Set TMS state
 *
 * @param value TMS logic level
 */
void jtag_set_tms(uint8_t value);

/**
 * @brief Set TDI state
 *
 * @param value TDI logic level
 */
void jtag_set_tdi(uint8_t value);

/**
 * @brief Get TDO state
 *
 * @return TDO logic level
 */
uint8_t jtag_get_tdo(void);

/**
 * @brief Set TRST state
 *
 * @param value TRST logic level
 */
void jtag_set_trst(uint8_t value);

/** 
 * @brief Set SRST state
 *
 * @param value SRST logic level
 */
void jtag_set_srst(uint8_t value);

/**
 * @brief Read and write multiple bits
 *
 * Read and write multiple bits. In order to save memory,
 * each bit of the in/out arrays are used to carry data.
 *
 * @param length Number of bits exchanged
 * @param in Input data
 * @param out Output data
 */
void jtag_transfer(uint16_t length, const uint8_t *in,
		   uint8_t *out);

void jtag_strobe(uint8_t pulses, bool tms, bool tdi);
