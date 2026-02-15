/* platform/c910/startup.S */
.section .init
.globl _start
.type _start, @function

_start:
    /* 1. Disable Interrupts */
    csrw mie, zero
    csrw sie, zero

    /* 2. Setup Trap Vector (CRITICAL FIX) 
       We set BOTH Machine (mtvec) and Supervisor (stvec) vectors 
       to ensure we catch the trap regardless of privilege mode.
    */
    //la t0, trap_handler
    csrw mtvec, t0
    csrw stvec, t0

    /* 3. Enable FPU (FS) and Vector (VS) */
    /* 0x01806000 sets VS=Initial(11) and FS=Initial(11) */
    li t1, 0x01806000 
    csrs mstatus, t1  /* Enable in Machine Mode */
    csrs sstatus, t1  /* Enable in Supervisor Mode */

    /* 4. Initialize Global Pointer */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* 5. Initialize Stack Pointer */
    la sp, _stack_top

    /* 6. Clear BSS */
    la t0, _sbss
    la t1, _ebss
    bge t0, t1, bss_done
bss_loop:
    sd zero, 0(t0)
    addi t0, t0, 8
    blt t0, t1, bss_loop
bss_done:

    /* 7. Jump to Main */
    call main

hang:
    j hang
