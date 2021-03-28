#include <stdint.h>
#include <string.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/timer.h>
#include <unicore-mx/cm3/nvic.h>
#include <unicore-mx/stm32/spi.h>

#include "jtag.h"

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

#elif PLATFORM == HW_stlinkv2white

#define JTAG_PORT_TCK GPIOA
#define JTAG_PIN_TCK GPIO5

#define JTAG_PORT_TDO GPIOA
#define JTAG_PIN_TDO GPIO6

#define JTAG_PORT_TDI GPIOA
#define JTAG_PIN_TDI GPIO7

#define JTAG_PORT_TMS GPIOB
#define JTAG_PIN_TMS GPIO14

#define JTAG_PORT_SRST GPIOB
#define JTAG_PIN_SRST GPIO0

#define JTAG_PORT_TRST GPIOB
#define JTAG_PIN_TRST GPIO1

#else /* Blue Pill platform (default) */

#define JTAG_PORT_TDI GPIOA
#define JTAG_PIN_TDI GPIO_SPI1_MOSI       /* PA7 */

#define JTAG_PORT_TDO GPIOA
#define JTAG_PIN_TDO GPIO_SPI1_MISO       /* PA6 */

#define JTAG_PORT_TCK GPIOA
#define JTAG_PIN_TCK GPIO_SPI1_SCK        /* PA5 */

#define JTAG_PORT_TMS GPIOA
#define JTAG_PIN_TMS GPIO3

#define JTAG_PORT_TRST GPIOA
#define JTAG_PIN_TRST GPIO4

#define JTAG_PORT_SRST GPIOA
#define JTAG_PIN_SRST GPIO2

#endif

#if !defined(JTAG_PORT_TDI) || \
  !defined(JTAG_PORT_TDO) || \
  !defined(JTAG_PORT_TMS) || \
  !defined(JTAG_PORT_TCK)
#error "Not enough pins defined for proper JTAG operation"
#endif

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
  gpio_set_mode(JTAG_PORT_TDO,
		GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_PULL_UPDOWN,
		JTAG_PIN_TDO);
  gpio_set_mode(JTAG_PORT_TMS,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		JTAG_PIN_TMS);
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

#if USE_SPI1
  /* SPI1 init */
  spi_reset(SPI1);
  spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_8, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1,
                  SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
  spi_enable_software_slave_management(SPI1);
  spi_set_nss_high(SPI1);
  spi_enable(SPI1);
#endif
  /* TIMER2 init */
  timer_reset(TIM2);

  timer_set_mode(TIM2,
                 TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_EDGE,
                 TIM_CR1_DIR_UP);

  timer_disable_counter(TIM2);

  timer_set_counter(TIM2,0);
  timer_set_prescaler(TIM2, 0);
  timer_continuous_mode(TIM2);
  timer_set_period(TIM2, 0x1);
  timer_disable_oc_output(TIM2, TIM_OC1);
  timer_disable_oc_output(TIM2, TIM_OC2);
  timer_disable_oc_output(TIM2, TIM_OC3);
  timer_disable_oc_output(TIM2, TIM_OC4);

  timer_disable_preload(TIM2);
  timer_enable_counter(TIM2);
}

bool max_frequency = true; //default is higest speed.

void jtag_set_frequency(uint32_t frequency) {
  /* Ensure that the frequency is within specs */
  if (frequency == 0) {
    frequency = 1;
  } else if (frequency > 1500) {
    frequency = 1500;
  }
  timer_disable_counter(TIM2);
  timer_set_counter(TIM2, 0);

  timer_set_prescaler(TIM2, 0);
  if (frequency == 1500)
  {
    timer_set_period(TIM2, 1);
  }
  else
  {
    timer_set_period(TIM2, F_CPU/(2*frequency*1000));
  }
  timer_enable_counter(TIM2);
  max_frequency = (frequency == 1500);
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





void jtag_transfer_internal(uint16_t length, const uint8_t *in, uint8_t *out) {
  uint32_t xfer_length, xfer_i;
  const uint8_t *xfer_in;
  uint8_t *xfer_out;
  /* Set TMS low during transfer */
  jtag_set_tms(0);
  /* Prepare transfer */
  xfer_length = length;
  xfer_i = 0;
  xfer_in = in;
  xfer_out = out;
  if (max_frequency)
  {
    while (xfer_i < xfer_length)
    {
      uint8_t bitmask;
      bitmask = 0x80 >> (xfer_i%8) ;

      if (xfer_in[xfer_i/8] & bitmask) {
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI;
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI;
      } else {
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI << 16;
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI << 16;
      }

      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK;

      if (GPIO_IDR(JTAG_PORT_TDO) & JTAG_PIN_TDO) {
        xfer_out[xfer_i/8] |= bitmask;
      }

      xfer_i++;


      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK << 16;
      
    }
  }
  else 
  {
    timer_set_counter(TIM2,0);
    TIM_SR(TIM2) = ~TIM_SR_UIF;

    while (xfer_i < xfer_length)
    {
      uint8_t bitmask;

      bitmask = 0x80 >> (xfer_i%8) ;

      if (xfer_in[xfer_i/8] & bitmask) {
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI;
      } else {
        GPIO_BSRR(JTAG_PORT_TDI) = JTAG_PIN_TDI << 16;
      }

      while (!(TIM_SR(TIM2) & TIM_SR_UIF));
      TIM_SR(TIM2) = ~TIM_SR_UIF;

      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK;

      while (!(TIM_SR(TIM2) & TIM_SR_UIF));
      TIM_SR(TIM2) = ~TIM_SR_UIF;

      if (GPIO_IDR(JTAG_PORT_TDO) & JTAG_PIN_TDO) {
        xfer_out[xfer_i/8] |= bitmask;
      }

      xfer_i++;

      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK << 16;
    

      
      

    }
  }

}

void jtag_strobe(uint8_t pulses, bool tms, bool tdi) {
  /* Set TMS state */
  jtag_set_tms(tms);
  jtag_set_tdi(tdi);
  if (max_frequency)
  {
    while (pulses)
    {
      //double the writes to slow down the clock rate
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK;
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK;
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK << 16;
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK << 16;
      pulses--;
    }
  }
  else
  {
    timer_set_counter(TIM2,0);
    while (pulses)
    {
      TIM_SR(TIM2) = ~TIM_SR_UIF;
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK;
      while (!(TIM_SR(TIM2) & TIM_SR_UIF));
      TIM_SR(TIM2) = ~TIM_SR_UIF;
      GPIO_BSRR(JTAG_PORT_TCK) = JTAG_PIN_TCK << 16;
      pulses--;
      while (!(TIM_SR(TIM2) & TIM_SR_UIF));
    }
  }
}

#if USE_SPI1
void jtag_transfer(uint16_t length, const uint8_t *in, uint8_t *out) {
  uint32_t byte_length = max_frequency ? length/8 : 0;
  uint16_t remaining_length = max_frequency ? length & 7 : length;
  uint32_t xfer_in_i = 0;
  uint32_t xfer_out_i = 0;

  if (max_frequency)
  {
    /* Set TMS low during transfer */
    jtag_set_tms(0);
    //set the pins in SPI mode
    gpio_set_mode(JTAG_PORT_TCK,
      GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
      JTAG_PIN_TCK | JTAG_PIN_TDI);
    
    while (xfer_out_i < byte_length)
    {

      /* if the transmit reg is empty, push a byte */
      /* this should make the transfer continuous  */ 
      if ((SPI_SR(SPI1) & SPI_SR_TXE) && (xfer_in_i < byte_length))
      {
        SPI_DR(SPI1) = in[xfer_in_i++];
      }
      /* Wait for received byte */
      if ((SPI_SR(SPI1) & SPI_SR_RXNE))
      {
        /* Read the data */
        out[xfer_out_i++] = SPI_DR(SPI1);
      }

    }
    
    //set pins in GPIO mode
    gpio_set_mode(JTAG_PORT_TCK,
      GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_PUSHPULL,
      JTAG_PIN_TCK | JTAG_PIN_TDI);
  }
  
  if (remaining_length)
    jtag_transfer_internal(remaining_length, &in[byte_length], &out[byte_length]);
}

#else
void jtag_transfer(uint16_t length, const uint8_t *in, uint8_t *out) {
  jtag_transfer_internal(length, in, out);
}


#endif
