#include <stdint.h>
#include <unicore-mx/stm32/gpio.h>

#include "delay.h"
#include "jtag.h"

#define HW_stlinkv2 0
#define HW_bluepill 1

#if PLATFORM == HW_stlinkv2 /* Chinese ST-LinkV2 clone */

#define JTAG_PORT_TDI GPIOB
#define JTAG_PIN_TDI GPIO8

#define JTAG_PORT_TDO GPIOB
#define JTAG_PIN_TDO GPIO14

#define JTAG_PORT_TCK GPIOA
#define JTAG_PIN_TCK GPIO5

#define JTAG_PORT_TMS GPIOB
#define JTAG_PIN_TMS GPIO6

#else /* Blue Pill platform (default) */

#define JTAG_PORT_TDI GPIOA
#define JTAG_PIN_TDI GPIO0

#define JTAG_PORT_TDO GPIOA
#define JTAG_PIN_TDO GPIO1

#define JTAG_PORT_TCK GPIOA
#define JTAG_PIN_TCK GPIO2

#define JTAG_PORT_TMS GPIOA
#define JTAG_PIN_TMS GPIO3

#define JTAG_PORT_TRST GPIOA
#define JTAG_PIN_TRST GPIO4

#define JTAG_PORT_SRST GPIOA
#define JTAG_PIN_SRST GPIO5

#endif

#if !defined(JTAG_PORT_TDI) || \
  !defined(JTAG_PORT_TDO) || \
  !defined(JTAG_PORT_TMS) || \
  !defined(JTAG_PORT_TCK)
#error "Not enough pins defined for proper JTAG operation"
#endif

static uint32_t period = 1;

void jtag_init(void) {
  /* GPIO configuration */
  gpio_set_mode(JTAG_PORT_TCK,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_PIN_TCK);
  gpio_set_mode(JTAG_PORT_TDI,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_PIN_TDI);
  gpio_set_mode(JTAG_PORT_TMS,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_PIN_TMS);
  gpio_set_mode(JTAG_PORT_TDO,
		GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_PULL_UPDOWN,
		JTAG_PIN_TDO);
#ifdef JTAG_PORT_SRST
  gpio_set_mode(JTAG_PORT_SRST,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_PIN_SRST);
#endif
#ifdef JTAG_PORT_TRST
  gpio_set_mode(JTAG_PORT_TRST,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_PIN_TRST);
#endif
#if PLATFORM == HW_stlinkv2
  /* Put weird STLinkV2 clone pull pins in high impedance state */
  gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_FLOAT,
		GPIO5 | GPIO7 | GPIO9 | GPIO10 | GPIO12);
#endif

  /* Set default pin state */
  gpio_clear(JTAG_PORT_TCK, JTAG_PIN_TCK);
  gpio_clear(JTAG_PORT_TMS, JTAG_PIN_TMS);
  gpio_clear(JTAG_PORT_TDI, JTAG_PIN_TDI);
#ifdef JTAG_PORT_TRST
  gpio_set(JTAG_PORT_TRST, JTAG_PIN_TRST);
#endif
#ifdef JTAG_PORT_SRST
  gpio_set(JTAG_PORT_SRST, JTAG_PIN_SRST);
#endif

  /* Set pull-down on TDO */
  gpio_clear(JTAG_PORT_TDO, JTAG_PIN_TDO);
}

void jtag_set_frequency(uint32_t frequency) {
  /* The period is set in microseconds */
  /* Frequency value is in kilohertz (kHz) */
  //period = 1000/frequency;
  (void)frequency;
}

void jtag_clock(void) {
  delay_us(period/2);
  
  gpio_set(JTAG_PORT_TCK, JTAG_PIN_TCK);

  delay_us(period);

  gpio_clear(JTAG_PORT_TCK, JTAG_PIN_TCK);
  delay_us(period/2);
}

void jtag_set_tck(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT_TCK, JTAG_PIN_TCK);
  } else {
    gpio_clear(JTAG_PORT_TCK, JTAG_PIN_TCK);
  }
}

void jtag_set_tms(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT_TMS, JTAG_PIN_TMS);
  } else {
    gpio_clear(JTAG_PORT_TMS, JTAG_PIN_TMS);
  }
}

void jtag_set_tdi(uint8_t value) {
  if (value) {
    gpio_set(JTAG_PORT_TDI, JTAG_PIN_TDI);
  } else {
    gpio_clear(JTAG_PORT_TDI, JTAG_PIN_TDI);
  }
}

uint8_t jtag_get_tdo(void) {
  return gpio_get(JTAG_PORT_TDO, JTAG_PIN_TDO) ? 1 : 0;
}

void jtag_set_trst(uint8_t value) {
#ifdef JTAG_PORT_TRST
  if (value) {
    gpio_set(JTAG_PORT_TRST, JTAG_PIN_TRST);
  } else {
    gpio_clear(JTAG_PORT_TRST, JTAG_PIN_TRST);
  }
#else
  (void)value;
#endif
}

void jtag_set_srst(uint8_t value) {
#ifdef JTAG_PORT_SRST
  if (value) {
    gpio_set(JTAG_PORT_SRST, JTAG_PIN_SRST);
  } else {
    gpio_clear(JTAG_PORT_SRST, JTAG_PIN_SRST);
  }
#else
  (void)value;
#endif
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

