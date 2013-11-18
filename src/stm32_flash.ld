ENTRY(Reset_Handler)

_estack = 0x20002000;    

_Min_Heap_Size = 0;      
_Min_Stack_Size = 0x100; 


MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 128K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 8K
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) 
    . = ALIGN(4);
  } >FLASH
  
  .text :
  {
    . = ALIGN(4);
    *(.text)           
    *(.text*)          
    *(.rodata)         
    *(.rodata*)        
    *(.glue_7)         
    *(.glue_7t)        

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        
  } >FLASH

   .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM : {
    __exidx_start = .;
      *(.ARM.exidx*)
      __exidx_end = .;
    } >FLASH

  .ARM.attributes : { *(.ARM.attributes) } > FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  
  _sidata = .;

  
  .data : AT ( _sidata )
  {
    . = ALIGN(4);
    _sdata = .;        
    *(.data)           
    *(.data*)          

    . = ALIGN(4);
    _edata = .;        
  } >RAM

  
  . = ALIGN(4);
  .bss :
  {
    
    _sbss = .;         
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         
    __bss_end__ = _ebss;
  } >RAM

  PROVIDE ( end = _ebss );
  PROVIDE ( _end = _ebss );

  
  ._user_heap_stack :
  {
    . = ALIGN(4);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM

  
  
  .memory_b1_text :
  {
    *(.mb1text)        
    *(.mb1text*)       
    *(.mb1rodata)      
    *(.mb1rodata*)
  } >MEMORY_B1

  
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
}
