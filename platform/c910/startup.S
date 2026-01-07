.section .init
.globl _start
.type _start, @function

_start:
    /* --------------------------------------------------------------------
       Supervisor Mode Setup (Booted from U-Boot)
       -------------------------------------------------------------------- */

    /* 1. Disable Interrupts (sie) */
    csrw sie, zero

    /* 2. Clear sstatus and Disable Vector Unit for safety 
       VS bits are [24:23]. 00 = Off, 01 = Initial, 11 = Dirty
       We clear everything to be safe. */
    csrw sstatus, zero

    /* 3. Initialize Global Pointer (gp)
       The linker uses this for relaxation optimization */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* 4. Initialize Stack Pointer (sp) 
       _stack_top is defined in linker.ld */
    la sp, _stack_top

    /* 5. Setup Trap Vector (stvec)
       Point to a default hang loop for now to catch early crashes */
    la t0, early_trap_handler
    csrw stvec, t0

    /* 6. Clear BSS (Zero out uninitialized globals) */
    la t0, _sbss
    la t1, _ebss
    bge t0, t1, bss_done
bss_loop:
    sd zero, 0(t0)
    addi t0, t0, 8
    blt t0, t1, bss_loop
bss_done:

    /* 7. Jump to C main */
    call main

    /* 8. Safety Hang */
hang:
    j hang

/* Early Trap Handler: If we crash before FreeRTOS, we loop here */
.align 4
early_trap_handler:
    j early_trap_handler
