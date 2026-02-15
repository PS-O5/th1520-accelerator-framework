#include <stdio.h>
#include "c910.h"

int main() {
    printf("=== C910 Polyfill Vector Test ===\n");

    // 1. Configure Vector Unit
    long vl = c910_vsetvli_e32m1(4);
    printf("Vector Length set to: %ld\n", vl);

    // 2. Broadcast 10 to v0
    printf("Broadcasting 10 to v0...\n");
    c910_vmv_v_x_v0(10);

    // 3. Add 1 to v0 (Hardware assumes immediate=1 for this test opcode)
    printf("Adding 1 to v0...\n");
    c910_vadd_vi_v0(1);

    // 4. Extract Result
    int result = c910_vmv_x_s_v0();
    printf("Result extracted: %d\n", result);

    if (result == 11) {
        printf("[SUCCESS] Hardware is Calculating Correctly!\n");
    } else {
        printf("[FAIL] Expected 11, got %d\n", result);
    }

    return 0;
}
