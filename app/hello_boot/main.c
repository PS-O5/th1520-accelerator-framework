#include "platform_conf.h"
#include "uart.h"

// Forward declare UART functions
void console_init(void);
void uart_puts(const char *s);

int main(void) {
    console_init();
    
    uart_puts("\n\n");
    uart_puts("=======================================\n");
    uart_puts("   Lichee Pi 4A (TH1520) Bare Metal    \n");
    uart_puts("   Accelerator Host Mode: Active       \n");
    uart_puts("=======================================\n");

    // Simple echo loop to test RX/TX
    // (Implementation of uart_getc needed for this part, 
    //  but for now just sending TX is enough to prove life)
    
    while(1) {
       // Loop forever
    }

    return 0;
}
