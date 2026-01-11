#include "uart.h"
#include <stdint.h>

// Forward declare the vector test
void test_vector_unit(void);

int main(void) {
    console_init(); 
    
    uart_puts("\n");
    uart_puts("=======================================\n");
    uart_puts("   Lichee Pi 4A: Vector Test           \n");
    uart_puts("   Enabling RVV 0.7.1...               \n");
    uart_puts("=======================================\n");

    // Run the test
    test_vector_unit();

    uart_puts("Test Complete. Hanging.\n");
    while(1);
    return 0;
}

void test_vector_unit(void) {
    // 1. Define two arrays of data
    uint64_t a[4] = {10, 20, 30, 40};
    uint64_t b[4] = {1,  2,  3,  4};
    uint64_t c[4] = {0,  0,  0,  0}; // Result

    uart_puts("Calculating: [10,20,30,40] + [1,2,3,4] using RVV...\n");

    /* * RVV 0.7.1 Assembly Breakdown:
     * vsetvli t0, a0, e64, m1  -> Configure vector length (4 elements, 64-bit width)
     * vle.v   v0, (a1)         -> Load vector a into v0
     * vle.v   v1, (a2)         -> Load vector b into v1
     * vadd.vv v2, v0, v1       -> Add v0 + v1, store in v2
     * vse.v   v2, (a3)         -> Store v2 into c
     */

    // We use a block to force register allocation
    long vl = 4; // Vector Length
    
    __asm__ volatile (
        "vsetvli t0, %0, e64, m1\n"  // Set Vector Length, 64-bit elements
        "vle.v   v0, (%1)\n"         // Load A
        "vle.v   v1, (%2)\n"         // Load B
        "vadd.vv v2, v0, v1\n"       // Add
        "vse.v   v2, (%3)\n"         // Store C
        : 
        : "r"(vl), "r"(a), "r"(b), "r"(c)
        : "t0", "v0", "v1", "v2", "memory"
    );

    // Verify Results
    int success = 1;
    for(int i=0; i<4; i++) {
        // We can't use printf, so we check logic manually
        if (c[i] != (a[i] + b[i])) {
            success = 0;
        }
    }

    if (success) {
        uart_puts("SUCCESS: Vector Addition Verified!\n");
        // Optional: Print one result to prove it
        // (Since we don't have printf %d yet, we trust the success flag)
    } else {
        uart_puts("FAILURE: Math check failed.\n");
    }
}
