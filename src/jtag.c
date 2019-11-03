#include <stdint.h>
#include <string.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/timer.h>
#include <unicore-mx/cm3/nvic.h>

#include "jtag.h"

#define F_CPU 72000000UL

#define HW_stlinkv2 0
#define HW_stlinkv2dfu 0
#define HW_bluepill 1
#define HW_olimexstm32h103 1
#define HW_baite 2

#if PLATFORM == HW_stlinkv2 /* Chinese ST-LinkV2 clone */

#define JTAG_PORT_TDI GPIOB
#define JTAG_PIN_TDI GPIO8

#define JTAG_PORT_TDO GPIOB
#define JTAG_PIN_TDO GPIO14

#define JTAG_PORT_TCK GPIOA
#define JTAG_PIN_TCK GPIO5

#define JTAG_PORT_TMS GPIOB
#define JTAG_PIN_TMS GPIO6

#elif PLATFORM == HW_baite

#define JTAG_PORT_TDI GPIOA
#define JTAG_PIN_TDI GPIO7

#define JTAG_PORT_TDO GPIOA
#define JTAG_PIN_TDO GPIO6

#define JTAG_PORT_TCK GPIOB
#define JTAG_PIN_TCK GPIO13

#define JTAG_PORT_TMS GPIOB
#define JTAG_PIN_TMS GPIO14

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

#if JTAG_PORT_TDI == JTAG_PORT_TDO && \
  JTAG_PORT_TDI == JTAG_PORT_TCK && \
  JTAG_PORT_TDI == JTAG_PORT_TMS
#define JTAG_UNIPORT
#endif

static const uint8_t xfer_const_1[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static const uint8_t xfer_const_0[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static volatile const uint8_t *xfer_in;
static volatile uint8_t *xfer_out;
static volatile uint16_t xfer_length, xfer_i;
static volatile bool xfer_clk_hi;
static volatile bool xfer_done;

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

  /* TIMER2 init */
  nvic_enable_irq(NVIC_TIM2_IRQ);

  timer_reset(TIM2);

  timer_set_mode(TIM2,
                 TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_EDGE,
                 TIM_CR1_DIR_UP);

  timer_disable_counter(TIM2);

  timer_set_counter(TIM2,0);
  timer_set_prescaler(TIM2, 1);
  timer_continuous_mode(TIM2);
  timer_set_period(TIM2, 360);
  timer_enable_irq(TIM2, TIM_DIER_UIE);

  timer_disable_oc_output(TIM2, TIM_OC1);
  timer_disable_oc_output(TIM2, TIM_OC2);
  timer_disable_oc_output(TIM2, TIM_OC3);
  timer_disable_oc_output(TIM2, TIM_OC4);

  timer_disable_preload(TIM2);
}

void jtag_set_frequency(uint32_t frequency) {
  /* Ensure that the frequency is within specs */
  if (frequency == 0) {
    frequency = 1;
  } else if (frequency > 1000) {
    frequency = 1000;
  }
  
  /* timer_disable_counter(TIM2); */
  /* timer_set_counter(TIM2, 0); */

  /* timer_set_prescaler(TIM2, 1); */
  /* timer_set_period(TIM2, F_CPU/(2*frequency*1000)); */
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
  /* Set TMS low during transfer */
  jtag_set_tms(0);

  /* Prepare transfer */
  xfer_length = length;
  xfer_i = 0;
  xfer_in = (uint8_t*)in;
  xfer_out = out;
  xfer_clk_hi = true;
  xfer_done = false;

  timer_enable_counter(TIM2);
  while (!xfer_done);
  timer_disable_counter(TIM2);

  gpio_set(GPIOC, GPIO13);
}

void jtag_strobe(uint8_t pulses, bool tms, bool tdi) {
  uint8_t buf1[8];

  /* Set TMS state */
  jtag_set_tms(tms);

  /* Fill output buffer */
  if (tdi) {
    xfer_in = xfer_const_1;
  } else {
    xfer_in = xfer_const_0;
  }

  xfer_length = pulses;
  xfer_i = 0;
  xfer_out = buf1;
  xfer_clk_hi = true;
  xfer_done = false;

  timer_enable_counter(TIM2);
  while (!xfer_done);
  timer_disable_counter(TIM2);
}

void tim2_isr(void) {
  uint8_t bitmask;
  
  if (timer_get_flag(TIM2, TIM_SR_UIF)) {
    if (xfer_clk_hi && xfer_i < xfer_length) {
      bitmask = 0x80 >> (xfer_i%8);
      xfer_out[xfer_i/8] |= gpio_get(JTAG_PORT_TDO, JTAG_PIN_TDO) ? bitmask : 0 ;

#ifdef JTAG_UNIPORT
      if (xfer_in[xfer_i/8] & bitmask) {
        GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK | JTAG_PIN_TDI;
      } else {
        GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK | (JTAG_PIN_TDI << 16);
      }
#else
      if (xfer_in[xfer_i/8] & bitmask) {
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI;
      } else {
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI << 16;
      }
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK;
#endif

      xfer_clk_hi = false;
      xfer_i++;
    } else {
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK << 16;
      xfer_clk_hi = true;

      if (xfer_i == xfer_length) {
        xfer_done = true;
      }
    }

    timer_clear_flag(TIM2, TIM_SR_UIF);
  }
}

