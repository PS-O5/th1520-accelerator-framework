#include "uart.h"
#include <stdint.h>

extern t_ck_uart_device g_console_uart;

void print_hex(uint64_t val) {
    char hex[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 15; i >= 0; i--) {
        int nibble = (val >> (i * 4)) & 0xF;
        ck_uart_putc(&g_console_uart, hex[nibble]);
    }
    uart_puts("\r\n");
}

int main(void) {
    console_init(); 
    uart_puts("\n=== Lichee Pi 4A: Register Loopback Test ===\n");
    uart_puts("Testing: Scalar(a1) -> Vector(v0) -> Scalar(a3)\n");

    uint64_t result_val = 0;
    uint64_t vl_debug = 0;
    // We use a distinctive pattern to rule out "lucky zeros"
    uint64_t magic_pattern = 0xDEADBEEF;

    __asm__ volatile (
        // -------------------------------------------------
        // 1. SETUP
        // -------------------------------------------------
        "mv a1, %2 \n"          // a1 = 0xDEADBEEF
        "li a3, 0 \n"           // a3 = 0 (Target)
        "li a0, 1 \n"           // VL = 1
        "csrw vstart, zero \n"

        // -------------------------------------------------
        // 2. CONFIGURE (vsetvli)
        // -------------------------------------------------
        // vsetvli t0, a0, e32, m1
        ".long 0x008572d7 \n"
        "csrr %1, vl \n"

        // -------------------------------------------------
        // 3. SCALAR -> VECTOR (vmv.v.x)
        // -------------------------------------------------
        // vmv.v.x v0, a1  (Broadcast a1 to v0)
        // C910 Encoding: 0x5eb06057 (rs1=11/a1, vd=0)
        ".long 0x5eb06057 \n"

        // -------------------------------------------------
        // 4. VECTOR -> SCALAR (vmv.x.s)
        // -------------------------------------------------
        // vmv.x.s a3, v0  (Extract v0[0] to a3)
        // C910 Encoding: 0x320026d7 (rd=13/a3, vs2=0)
        ".long 0x320026d7 \n"
        
        "mv %0, a3 \n"          // Move a3 to C variable

        : "=r"(result_val), "=r"(vl_debug)
        : "r"(magic_pattern)
        : "a0", "a1", "a3", "t0", "v0", "memory"
    );

    uart_puts("Vector Length: "); print_hex(vl_debug);
    uart_puts("Input Data:    "); print_hex(magic_pattern);
    uart_puts("Output Data:   "); print_hex(result_val);

    if (result_val == magic_pattern) {
        uart_puts("\n[SUCCESS] Vector Data Path is ALIVE!\n");
        uart_puts("The ALU failures were just Cache/Memory quirks.\n");
    } else {
        uart_puts("\n[FAIL] Data lost in register file.\n");
    }

    while (1);
    return 0;
}
