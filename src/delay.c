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

#include <unicore-mx/cm3/systick.h>
#include <unicore-mx/stm32/gpio.h>
#include <stdint.h>

static bool wait_for_irq;

void delay_init(void) {
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
}

void _internal_delay_us(const uint32_t delay_val) {
  wait_for_irq = true;

  systick_set_reload(delay_val);
  STK_CVR = 0;
  systick_interrupt_enable();
  systick_counter_enable();

  //while (wait_for_irq);
}

void sys_tick_handler(void) {
  //wait_for_irq = false;
  //systick_interrupt_disable();
  gpio_toggle(GPIOA, GPIO5);
}
