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

#define _DELAY_F_CPU 72000000ULL

/**
 * @brief Initialises delay peripheral
 *
 * Halts code execution for a specified amount of time
 * expressed in microseconds. The function was copied
 * from the superbitrf-firmware project.
 *
 */
void delay_init(void);

/**
 * @brief Halts code executation
 *
 * Halts code execution for a specified amount of time
 * expressed in microseconds. This function is blocking.
 * Its accuracy is okay-ish.
 *
 * @param delay Delay in microseconds
 */
void _internal_delay_us(const uint32_t delay_val);

#define delay_us(X) _internal_delay_us(_DELAY_F_CPU * X / 1000000)
