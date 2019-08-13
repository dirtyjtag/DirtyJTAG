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

#include <unicore-mx/stm32/rcc.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/iwdg.h>
#include <unicore-mx/stm32/st_usbfs.h>
#include <unicore-mx/cm3/nvic.h>

#include "jtag.h"
#include "usb.h"
#include "delay.h"

#define HW_bluepill 0
#define HW_stlinkv2 1
#define HW_stlinkv2dfu 2
#define HW_baite 3

void clean_nvic(void) {
  uint8_t i;

  /* Reset all IRQs. This is necessary on ST-Linkv2 DFU target
   because the bootloader would handle USB transaction instead
   of DirtyJTAG */
  for (i = 0; i < 255; i++) {
    nvic_disable_irq(i);
  }
}

int main(void) {
  /* Clock init */
  rcc_clock_setup_in_hsi_out_48mhz();

  /* ST-Link v2 specific */
#if PLATFORM == HW_stlinkv2dfu
  /* Resetting IRQs */
  clean_nvic();

  /* Turn off USB controller */
  USB_CNTR = USB_CNTR_FRES;
  USB_ISTR = 0;
  USB_CNTR |= USB_CNTR_PDWN;

  /* Resetting peripherals */
  rcc_periph_reset_pulse(RST_TIM1);
  rcc_periph_reset_pulse(RST_TIM2);
  rcc_periph_reset_pulse(RST_TIM3);
  rcc_periph_reset_pulse(RST_AFIO);
  rcc_periph_reset_pulse(RST_USB);

  /* Disable watchdog */
  IWDG_KR = 0;
#endif
  
  /* Peripherals reset/init */
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_AFIO);

  /* Disable DirtyJTAG's own JTAG interface */
  AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON;

  /* Force USB to reenumerate (bootloader exit, SWD flashing, etc.) */
  usb_reenumerate();
  rcc_periph_reset_pulse(RST_GPIOA);
  rcc_periph_reset_pulse(RST_USB);

  delay_us(100);

  jtag_init();

  /* Turn on the onboard LED */
#if PLATFORM == HW_bluepill
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
    GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
  gpio_set(GPIOC, GPIO13);
#elif PLATFORM == HW_stlinkv2 || PLATFORM == HW_stlinkv2dfu
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
    GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);
  gpio_set(GPIOA, GPIO9);
#elif PLATFORM == HW_baite
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);
  gpio_clear(GPIOA, GPIO9);
#endif
  
  usb_init();
  
  return 0;
}
