OUTPUT_ARCH(i386:x86-64)
OUTPUT_FORMAT(elf64-x86-64)

ENTRY (_start)


MEMORY
{   
    MULTIBOOT(r)        : ORIGIN = 0x00001000, LENGTH = 8K
    CODE(rx)            : ORIGIN = 0x00100000, LENGTH = 64M
    RAM(rw)             : ORIGIN = 0x04100000, LENGTH = 64M
    HEAP (rw)           : ORIGIN = 0x08100000, LENGTH = 64M
    DATA_ROM (r)        : ORIGIN = 0x0C100000, LENGTH = 64M
}



SECTIONS
{
  .multiboot :
  {
   /* Ensure that this segment is within the 8K range, otherwise the executable 
    * file ''multiboot-elf" will not be recognized.
    */
    . = ALIGN(4);
    KEEP(*(.multiboot_header))
  } > MULTIBOOT
  
  .text : 
  {
     __code_start = .;
     . = ALIGN(4);
     *(.text)
  } > CODE
  
  .isr_text : {
     *(.isr)
  } > CODE
  
  .gdt386 :
  { 
    . = ALIGN(8);
    KEEP(*(.x86_gdt_section))
  } > CODE
  
  .rodata : 
  {
    __rodata_start = .;
    *(.rodata)
    *(.rodata.*)
    __rodata_end = .;
  } > CODE
  
  __code_end = .;
  
  /* Used by the startup to initialize data */
  __load_data_start = LOADADDR(.data);
  
  .data : 
  {
    __data_start = .;
    *(.data)
    *(.data.*)
    __data_end = .;
  } > RAM AT > CODE
  
  .bss : 
  {
    __bss_start = .;
    *(COMMON)
    *(.bss)
    *(.bss.*)
    __bss_end = .;
  } > RAM
 
  
  __stack_start = .;
  .stack : {
    KEEP(*(.kernel.stack))
  } > RAM
  __stack_end = .;
  
  .heap : {
      . = ALIGN(64);
      __malloc_start = .;
  } > HEAP
}


PROVIDE(kernel_code_start = __code_start);
PROVIDE(kernel_code_end = __code_start + LENGTH(CODE));

PROVIDE(rodata_start = __rodata_start);
PROVIDE(rodata_end = __rodata_end);

PROVIDE(load_data_start = __load_data_start);

PROVIDE(data_start = __data_start);
PROVIDE(data_end = __data_end);

PROVIDE(bss_start = __bss_start);
PROVIDE(bss_end = __bss_end);

PROVIDE(stack_start = __stack_start);
PROVIDE(stack_end = __stack_end);

PROVIDE(malloc_region_start = __malloc_start);
PROVIDE(malloc_region_size = LENGTH(HEAP));

