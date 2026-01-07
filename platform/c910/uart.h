#ifndef __UART_H__
#define __UART_H__

// REPLACE "datatype.h" with standard includes
#include <stdint.h>
#include "platform_conf.h" // Ensures we get the correct UART0_BASE_ADDR

/* * CRITICAL: We removed UART0_BASE_ADDR from here.
 * It is now defined in platform_conf.h or config.h as 0xFFE7014000UL
 */

/* UART register INDEX definitions 
 * (These are indices for uint32_t arrays, not byte offsets)
 */
#define CK_UART_RBR       0x00    /* Receive Buffer Register */
#define CK_UART_THR       0x00    /* Transmit Holding Register */
#define CK_UART_DLL       0x00    /* Divisor Latch(Low) */
#define CK_UART_IER       0x01    /* Interrupt Enable Register */
#define CK_UART_DLH       0x01    /* Divisor Latch(High) */
#define CK_UART_IIR       0x02    /* Interrupt Identity Register */
#define CK_UART_FCR       0x02    /* FIFO Control Register */
#define CK_UART_LCR       0x03    /* Line Control Register */
#define CK_UART_MCR       0x04    /* Modem Control Register */
#define CK_UART_LSR       0x05    /* Line Status Register */
#define CK_UART_MSR       0x06    /* Modem Status Register */
#define CK_UART_USR       0x1f    /* UART Status Register */

/* Enums for Configuration */
typedef enum {
    STOPBIT_1,
    STOPBIT_2
} t_ck_uart_stopbit;

typedef enum {
    PARITY_NONE,
    PARITY_ODD,
    PARITY_EVEN
} t_ck_uart_parity;

typedef enum {
    WORDSIZE_5,
    WORDSIZE_6,
    WORDSIZE_7,
    WORDSIZE_8,
    WORDSIZE_9
} t_ck_uart_wordsize;

typedef enum {
    DISABLE,
    ENABLE
} t_ck_uart_mode;

/* Structs */
typedef struct {
    uint32_t            baudrate;
    t_ck_uart_stopbit   stopbit;
    t_ck_uart_parity    parity;
    t_ck_uart_wordsize  wordsize;
    t_ck_uart_mode      rxmode;
    t_ck_uart_mode      txmode;
} t_ck_uart_cfig, *p_ck_uart_cfig;

typedef struct {
    uint32_t uart_id;
    uint32_t* register_map; /* Changed to uint32_t* for 32-bit stride access */
    uint32_t baudrate;
    t_ck_uart_parity parity;
    t_ck_uart_stopbit stopbit;
    t_ck_uart_wordsize wordsize;
    t_ck_uart_mode rxmode;
    t_ck_uart_mode txmode;
} t_ck_uart_device, *p_ck_uart_device;

/* Function Prototypes */
uint32_t ck_uart_open(p_ck_uart_device uart_device, uint32_t id);
uint32_t ck_uart_close(p_ck_uart_device uart_device);
uint32_t ck_uart_init(p_ck_uart_device uart_device, p_ck_uart_cfig uart_cfig);
uint32_t ck_uart_putc(p_ck_uart_device uart_device, uint8_t c);
uint32_t ck_uart_status(p_ck_uart_device uart_device);

void console_init(void);
void uart_puts(const char *s);

#endif /* __UART_H__ */
