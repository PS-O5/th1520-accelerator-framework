OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
  /* Adjust ORIGIN and LENGTH to match your hardware/simulator */
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  .text :
  {
    . = ALIGN(4);
    KEEP(*(.vector_table)) /* Keep the vector table if you define one */
    *(.text .text.*)
    *(.rodata .rodata.*)
    . = ALIGN(4);
  } > RAM

  .data :
  {
    . = ALIGN(16);
    _sdata = .;
    *(.data .data.*)
    *(.sdata .sdata.*)
    . = ALIGN(16);
    _edata = .;
  } > RAM

  .bss :
  {
    . = ALIGN(16);
    _sbss = .;
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    . = ALIGN(16);
    _ebss = .;
  } > RAM

  /* Stack and Heap definitions */
  . = ALIGN(16);
  _end = .;
  PROVIDE( _heap_start = . );
  
  /* Place stack at end of RAM */
  PROVIDE( _stack_top = ORIGIN(RAM) + LENGTH(RAM) );
}
