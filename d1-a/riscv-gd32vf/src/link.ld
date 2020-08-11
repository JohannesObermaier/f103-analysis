OUTPUT_ARCH( "riscv" )
ENTRY("entry")
MEMORY
{ 
  FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  SRAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 32K 
}


SECTIONS
{
  _stack = ORIGIN(SRAM) + LENGTH(SRAM);
  _flash_start = ORIGIN(FLASH);
  _flash_size = LENGTH(FLASH);
  __stack_size = DEFINED(__stack_size) ? __stack_size : 1K;
  __global_pointer$ = ORIGIN(SRAM);
  _global_pointer = ORIGIN(SRAM);

  .text :
  {
	_start_vectors = .;
	*(.vector_table)
	_tbl_end = .;
	. = ALIGN(8);
    *(.rodata .rodata.*)  
    *(.srodata .srodata.*)
    *(.text .text.*)
    
     /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)
  } >SRAM

  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);

}