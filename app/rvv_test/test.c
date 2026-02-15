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
}

void print_dec(uint64_t val) {
    char buf[20];
    char *ptr = &buf[19];
    *ptr = '\0';
    
    if (val == 0) {
        uart_puts("0");
        return;
    }
    
    while (val > 0) {
        *--ptr = '0' + (val % 10);
        val /= 10;
    }
    uart_puts(ptr);
}

/* Trap Handler */
volatile uint64_t trap_cause = 0;
volatile uint64_t trap_epc = 0;
volatile uint64_t trap_tval = 0;

__attribute__((naked, aligned(64)))
void trap_handler(void) {
    __asm__ volatile (
        // Save context
        "csrrw sp, mscratch, sp\n"
        "addi sp, sp, -128\n"
        "sd ra, 0(sp)\n"
        "sd t0, 8(sp)\n"
        "sd t1, 16(sp)\n"
        "sd t2, 24(sp)\n"
        
        // Read trap information
        "csrr t0, mcause\n"
        "csrr t1, mepc\n"
        "csrr t2, mtval\n"
        
        // Store in global variables (use absolute addresses)
        "lla t3, trap_cause\n"
        "sd t0, 0(t3)\n"
        "lla t3, trap_epc\n"
        "sd t1, 0(t3)\n"
        "lla t3, trap_tval\n"
        "sd t2, 0(t3)\n"
        
        // If illegal instruction, skip it
        "li t3, 2\n"
        "bne t0, t3, no_skip\n"
        "addi t1, t1, 4\n"
        "csrw mepc, t1\n"
        "no_skip:\n"
        
        // Restore context
        "ld ra, 0(sp)\n"
        "ld t0, 8(sp)\n"
        "ld t1, 16(sp)\n"
        "ld t2, 24(sp)\n"
        "addi sp, sp, 128\n"
        "csrrw sp, mscratch, sp\n"
        "mret\n"
    );
}

/* Check C910-specific features */
void probe_c910_features(void) {
    uart_puts("\n=== T-Head C910 Specific Features ===\n");
    
    // 1. Check mxstatus (0x7C0) - T-Head machine extended status
    uint64_t mxstatus;
    __asm__ volatile ("csrr %0, 0x7c0" : "=r"(mxstatus));
    uart_puts("mxstatus (0x7C0): "); print_hex(mxstatus); uart_puts("\n");
    
    // Bit 22 in mxstatus often controls vector extension
    if (mxstatus & (1 << 22)) {
        uart_puts("  - Vector extension bit (22) is ENABLED\n");
    } else {
        uart_puts("  - Vector extension bit (22) is DISABLED\n");
        uart_puts("  Attempting to enable...\n");
        mxstatus |= (1 << 22);
        __asm__ volatile ("csrw 0x7c0, %0" : : "r"(mxstatus));
        __asm__ volatile ("csrr %0, 0x7c0" : "=r"(mxstatus));
        uart_puts("  New mxstatus: "); print_hex(mxstatus); uart_puts("\n");
    }
    
    // 2. Check mfeaturesen (0x7C1) - Feature enable register
    uint64_t mfeaturesen;
    __asm__ volatile ("csrr %0, 0x7c1" : "=r"(mfeaturesen));
    uart_puts("mfeaturesen (0x7C1): "); print_hex(mfeaturesen); uart_puts("\n");
    
    // Bits [24:23] in mfeaturesen for vector
    if ((mfeaturesen >> 23) & 0x3) {
        uart_puts("  - Vector features are enabled\n");
    } else {
        uart_puts("  - Vector features are disabled\n");
    }
    
    // 3. Check mvendorid, marchid, mimpid
    uint64_t mvendorid, marchid, mimpid;
    __asm__ volatile (
        "csrr %0, mvendorid\n"
        "csrr %1, marchid\n"
        "csrr %2, mimpid\n"
        : "=r"(mvendorid), "=r"(marchid), "=r"(mimpid)
    );
    
    uart_puts("\nCore Identification:\n");
    uart_puts("  mvendorid: "); print_hex(mvendorid); 
    if (mvendorid == 0x5B7) uart_puts(" (T-Head)\n");
    else uart_puts(" (Unknown)\n");
    
    uart_puts("  marchid:   "); print_hex(marchid); uart_puts("\n");
    uart_puts("  mimpid:    "); print_hex(mimpid); uart_puts("\n");
}

/* Main vector probe for RVV 0.7.1 */
int main(void) {
    console_init(); 
    uart_puts("\n=== TH1520 (T-Head C910 RVV 0.7.1) Vector Probe ===\n");
    uart_puts("Core: Xuantie C910 with RVV 0.7.1\n");
    uart_puts("NPU: 4 TOPS @ 1GHz, GPU: BXM-4-64\n\n");
    
    // Setup trap handler first - FIXED: use register instead of immediate
    __asm__ volatile ("csrw mtvec, %0" : : "r"(&trap_handler));
    
    // Setup mscratch - FIXED: can't use large immediate with csrwi
    uint64_t scratch_val = 0x40010000;  // Safe memory area
    __asm__ volatile ("csrw mscratch, %0" : : "r"(scratch_val));
    
    // 1. Check MISA for V extension
    uart_puts("1. Checking MISA for 'V' extension...\n");
    uint64_t misa;
    __asm__ volatile ("csrr %0, misa" : "=r"(misa));
    uart_puts("   MISA: "); print_hex(misa); uart_puts("\n");
    
    int has_vector = (misa >> ('V' - 'A')) & 1;
    if (has_vector) {
        uart_puts("   ✓ V extension is in MISA\n");
    } else {
        uart_puts("   ✗ V extension NOT in MISA (but may still be present)\n");
    }
    
    // 2. Probe C910 specific features
    probe_c910_features();
    
    // 3. Check mstatus VS bits
    uart_puts("\n2. Checking mstatus vector state...\n");
    uint64_t mstatus;
    __asm__ volatile ("csrr %0, mstatus" : "=r"(mstatus));
    uart_puts("   Initial mstatus: "); print_hex(mstatus); uart_puts("\n");
    
    // Check VS bits (24:23)
    int vs_state = (mstatus >> 23) & 0x3;
    const char *vs_states[] = {"Off", "Initial", "Clean", "Dirty"};
    uart_puts("   VS state: "); uart_puts(vs_states[vs_state]); uart_puts("\n");
    
    // Try to set VS to dirty
    if (vs_state == 0) {
        uart_puts("   Setting VS to dirty (3)...\n");
        mstatus |= (3ULL << 23);
        __asm__ volatile ("csrw mstatus, %0" : : "r"(mstatus));
        __asm__ volatile ("fence.i");
        __asm__ volatile ("csrr %0, mstatus" : "=r"(mstatus));
        vs_state = (mstatus >> 23) & 0x3;
        uart_puts("   New VS state: "); uart_puts(vs_states[vs_state]); uart_puts("\n");
    }
    
    // 4. Try to access vector CSRs (RVV 0.7.1)
    uart_puts("\n3. Probing RVV 0.7.1 CSRs...\n");
    
    // Reset trap flag
    trap_cause = 0;
    
    // vtype (0xC21) - should be readable if vectors enabled
    uint64_t vtype = 0xDEADBEEF;
    __asm__ volatile (
        ".option push\n"
        ".option norvc\n"
        "csrr %0, 0xC21\n"  // vtype CSR
        ".option pop\n"
        : "=r"(vtype)
    );
    
    if (trap_cause == 0) {
        uart_puts("   vtype (0xC21): "); print_hex(vtype); uart_puts(" ✓\n");
        
        // Decode vtype
        int vsew = (vtype >> 3) & 0x7;  // SEW encoding
        int vlmul = vtype & 0x7;        // LMUL encoding (different in 0.7.1!)
        
        uart_puts("   Decoded: SEW=");
        if (vsew == 0) uart_puts("8");
        else if (vsew == 1) uart_puts("16");
        else if (vsew == 2) uart_puts("32");
        else if (vsew == 3) uart_puts("64");
        else print_dec(vsew);
        
        uart_puts(" LMUL=");
        if (vlmul == 0) uart_puts("1");
        else if (vlmul == 1) uart_puts("2");
        else if (vlmul == 2) uart_puts("4");
        else if (vlmul == 3) uart_puts("8");
        else print_dec(vlmul);
        uart_puts("\n");
    } else {
        uart_puts("   ✗ Failed to read vtype (trap)\n");
    }
    
    // vl (0xC20)
    trap_cause = 0;
    uint64_t vl = 0;
    __asm__ volatile (
        ".option push\n"
        ".option norvc\n"
        "csrr %0, 0xC20\n"  // vl CSR
        ".option pop\n"
        : "=r"(vl)
    );
    
    if (trap_cause == 0) {
        uart_puts("   vl (0xC20):    "); print_hex(vl); 
        uart_puts(" (max vector length="); print_dec(vl); uart_puts(") ✓\n");
    } else {
        uart_puts("   ✗ Failed to read vl\n");
    }
    
    // 5. Try a simple vector instruction
    uart_puts("\n4. Testing vector instructions...\n");
    
    // Try vsetvli (RVV 0.7.1)
    trap_cause = 0;
    uint64_t result = 0;
    
    __asm__ volatile (
        ".option push\n"
        ".option norvc\n"
        "li t0, 256\n"          // Request 256 elements
        "vsetvli %0, t0, e8, m1, ta, ma\n"  // Set SEW=8, LMUL=1
        ".option pop\n"
        : "=r"(result)
        :: "t0"
    );
    
    if (trap_cause == 0) {
        uart_puts("   vsetvli executed! Result (vl): "); 
        print_dec(result); uart_puts(" ✓\n");
        
        // Now try a memory operation if we have working memory
        uart_puts("\n5. Testing vector load/store...\n");
        
        // Setup a simple memory buffer - ensure alignment
        static uint8_t src[64] __attribute__((aligned(64)));
        static uint8_t dst[64] __attribute__((aligned(64)));
        
        for (int i = 0; i < 64; i++) {
            src[i] = i;
            dst[i] = 0;
        }
        
        trap_cause = 0;
        
        // Get pointers to pass to assembly
        uint8_t *src_ptr = src;
        uint8_t *dst_ptr = dst;
        
        __asm__ volatile (
            ".option push\n"
            ".option norvc\n"
            "mv t0, %0\n"      // Source address in register
            "mv t1, %1\n"      // Destination address in register
            "li t2, 32\n"      // 32 elements
            "vsetvli t3, t2, e8, m1, ta, ma\n"
            "vle8.v v0, (t0)\n"   // Load
            "vse8.v v0, (t1)\n"   // Store
            ".option pop\n"
            :: "r"(src_ptr), "r"(dst_ptr) 
            : "t0", "t1", "t2", "t3", "v0", "memory"
        );
        
        if (trap_cause == 0) {
            // Verify data
            int ok = 1;
            for (int i = 0; i < 32; i++) {
                if (dst[i] != src[i]) {
                    ok = 0;
                    break;
                }
            }
            
            if (ok) {
                uart_puts("   ✓ Vector load/store working!\n");
                uart_puts("\n*** SUCCESS: RVV 0.7.1 IS FULLY OPERATIONAL ***\n");
            } else {
                uart_puts("   ✗ Data mismatch\n");
            }
        } else {
            uart_puts("   ✗ Vector memory op trapped\n");
        }
    } else {
        uart_puts("   ✗ vsetvli trapped (cause="); 
        print_hex(trap_cause); uart_puts(")\n");
    }
    
   
    //while (1);
    return 0;
}
