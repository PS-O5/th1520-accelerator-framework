OUTPUT_ARCH( "riscv" )
ENTRY( _start )  /* Ensure your startup.S uses ".globl _start" */

MEMORY
{
  /* * TH1520 RAM Layout for U-Boot Payloads:
   * 0x00000000 - 0x001FFFFF : OpenSBI (DO NOT TOUCH)
   * 0x00200000 - ...        : Safe Zone
   */
  DRAM (xrw) : ORIGIN = 0x00200000, LENGTH = 128M 
}

SECTIONS
{
  /* Place startup code at the very beginning of the safe zone */
  .text :
  {
    . = ALIGN(8);
    KEEP(*(.init))   /* Defines the entry point location */
    KEEP(*(.text.startup))
    *(.text .text.*)
    . = ALIGN(8);
  } > DRAM

  .rodata :
  {
    . = ALIGN(8);
    *(.rodata .rodata.*)
    *(.srodata .srodata.*)
    . = ALIGN(8);
  } > DRAM

  .data :
  {
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    _sdata = .;
    *(.data .data.*)
    *(.sdata .sdata.*)
    . = ALIGN(8);
    _edata = .;
  } > DRAM

  .bss :
  {
    . = ALIGN(8);
    _sbss = .;
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(8);
    _ebss = .;
  } > DRAM

  /* * Stack Definitions
   * We place the stack at the very end of the 128MB block 
   * so it grows downwards without hitting our code.
   */
  . = ALIGN(16);
  _stack_top = ORIGIN(DRAM) + LENGTH(DRAM);
  
  /* For compatibility with some legacy assembly that looks for __kernel_stack */
  PROVIDE(__kernel_stack = _stack_top); 
}
