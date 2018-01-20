/* Generic linker script for STM32F103RBT6/STM32F103R8T6 */
MEMORY {
       rom (rx) : ORIGIN = 0x08000000, LENGTH = 128K
       ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE libucmx_stm32f1.ld
