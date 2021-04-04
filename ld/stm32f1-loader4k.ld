/* Generic STM32F1, application at 0x4000 */
MEMORY {
       rom (rx) : ORIGIN = 0x08004000, LENGTH = 128K-0x4000
       ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE libucmx_stm32f1.ld
