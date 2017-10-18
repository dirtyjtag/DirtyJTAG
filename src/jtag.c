#include <stdint.h>
#include <unicore-mx/stm32/gpio.h>

#include "delay.h"
#include "jtag.h"

#define JTAG_PORT GPIOA
#define JTAG_TDI GPIO0
#define JTAG_TDO GPIO1
#define JTAG_TCK GPIO2
#define JTAG_TMS GPIO3
#define JTAG_TRST GPIO4
#define JTAG_SRST GPIO5

static uint32_t period = 1;

void jtag_init(void) {
  /* GPIO configuration */
  gpio_set_mode(JTAG_PORT,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_TCK | JTAG_TDI | JTAG_TMS | JTAG_SRST | JTAG_TRST);
  gpio_set_mode(JTAG_PORT,
		GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_PULL_UPDOWN,
		JTAG_TDO);

  /* Set default pin state */
  gpio_clear(JTAG_PORT, JTAG_TCK | JTAG_TMS | JTAG_TDI);
  gpio_set(JTAG_PORT, JTAG_TRST | JTAG_SRST);

  /* Set pull-down on TDO */
  gpio_clear(JTAG_PORT, JTAG_TDO);
}

void jtag_set_frequency(uint32_t frequency) {
  /* The period is set in microseconds */
  /* Frequency value is in kilohertz (kHz) */
  //period = 1000/frequency;
  (void)frequency;
}

void jtag_clock(void) {
  delay_us(period/2);
  
  gpio_set(JTAG_PORT, JTAG_TCK);

  delay_us(period);

  gpio_clear(JTAG_PORT, JTAG_TCK);
  delay_us(period/2);
}

void jtag_set_tck(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT, JTAG_TCK);
  } else {
    gpio_clear(JTAG_PORT, JTAG_TCK);
  }
}

void jtag_set_tms(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT, JTAG_TMS);
  } else {
    gpio_clear(JTAG_PORT, JTAG_TMS);
  }
}

void jtag_set_tdi(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT, JTAG_TDI);
  } else {
    gpio_clear(JTAG_PORT, JTAG_TDI);
  }
}

uint8_t jtag_get_tdo(void) {
  return gpio_get(JTAG_PORT, JTAG_TDO) ? 1 : 0;
}

void jtag_set_trst(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT, JTAG_TRST);
  } else {
    gpio_clear(JTAG_PORT, JTAG_TRST);
  }
}

void jtag_set_srst(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT, JTAG_SRST);
  } else {
    gpio_clear(JTAG_PORT, JTAG_SRST);
  }
}

void jtag_transfer(uint16_t length, const uint8_t *in, uint8_t *out) {
  uint16_t i;

  /* Set TMS low during transfer */
  jtag_set_tms(0);

  /* Read TDO, then set TDI, then clock */
  for (i = 0; i < length; i++) {
    out[i/8] |= jtag_get_tdo() ? (0x80 >> (i%8)) : 0 ;
    
    jtag_set_tdi(in[i/8] & (0x80 >> (i%8)));
    
    jtag_clock();
  }
}

