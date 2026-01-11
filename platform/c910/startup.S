.section .init
.globl _start
.type _start, @function

_start:
    /* 1. Disable Interrupts */
    csrw sie, zero

    /* 2. Setup sstatus (Status Register) */
    /* FS (Float Status)  is bits [14:13] - Always enable FPU (0x00006000)
       VS (Vector Status) is bits [24:23] - Only enable if requested
    */
    
    /* Start with FPU enabled (Value: 0x00006000) */
    li t0, 0x00006000 

#ifdef ENABLE_VECTORS
    /* OR in the Vector Status bits (Value: 0x01800000) */
    li t1, 0x01800000
    or t0, t0, t1
#endif

    csrs sstatus, t0

    /* 3. Initialize Global Pointer */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* 4. Initialize Stack Pointer */
    la sp, _stack_top

    /* 5. Clear BSS */
    la t0, _sbss
    la t1, _ebss
    bge t0, t1, bss_done
bss_loop:
    sd zero, 0(t0)
    addi t0, t0, 8
    blt t0, t1, bss_loop
bss_done:

    /* 6. Jump to Main */
    call main

hang:
    j hang
