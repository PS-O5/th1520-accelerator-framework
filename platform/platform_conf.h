#ifndef PLATFORM_CONF_H
#define PLATFORM_CONF_H

#include <stdint.h>

/* TH1520 / Lichee Pi 4A Hardware Definitions */
#define UART0_BASE_ADDR     0xFFE7014000UL

/* Type Definitions for Drivers */
typedef volatile uint32_t reg32_t;
typedef volatile uint8_t  reg8_t;

#endif /* PLATFORM_CONF_H */
