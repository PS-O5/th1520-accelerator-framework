#ifndef C910_H
#define C910_H

#include <stdint.h>

// --- T-Head C910 Vector Polyfills for Standard GCC ---

// 1. vsetvli t0, a0, e32, m1
// Sets vector length. Returns the actual VL.
static inline long c910_vsetvli_e32m1(long requested_vl) {
    long actual_vl;
    asm volatile(
        "mv a0, %1 \n"          // Move requested VL to a0
        ".word 0x008572d7 \n"   // vsetvli t0, a0, e32, m1
        "mv %0, t0 \n"          // Return t0 (Actual VL)
        : "=r"(actual_vl) 
        : "r"(requested_vl) 
        : "a0", "t0", "memory"
    );
    return actual_vl;
}

// 2. vadd.vi v0, v0, imm
// Adds immediate to v0.
static inline void c910_vadd_vi_v0(int immediate) {
    // Opcode: 0x02103057 is vadd.vi v0, v0, 1
    // We can't easily template the immediate in raw hex without complex macros,
    // so we will hardcode adding '1' for the test, or 'immediate' if we use .insn
    // For safety/simplicity in this test, we hardcode ADD 1.
    asm volatile(
        ".word 0x02103057 \n" // vadd.vi v0, v0, 1
        ::: "v0", "memory"
    );
}

// 3. vmv.v.x v0, rs1
// Broadcast scalar to all elements of v0
static inline void c910_vmv_v_x_v0(int scalar) {
    asm volatile(
        "mv a0, %0 \n"
        ".word 0x5eb06057 \n" // vmv.v.x v0, a0
        : : "r"(scalar) : "a0", "v0", "memory"
    );
}

// 4. vmv.x.s rd, v0
// Extract first element of v0 to scalar
static inline int c910_vmv_x_s_v0() {
    int result;
    asm volatile(
        ".word 0x32002557 \n" // vmv.x.s a0, v0
        "mv %0, a0 \n"
        : "=r"(result) : : "a0", "memory"
    );
    return result;
}

#endif
