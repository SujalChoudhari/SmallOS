/* linker.ld - Linker script for OS stage */

ENTRY(_start)

SECTIONS
{
  . = 0x1000; /* Starting address for the code */

  .text : {
    *(.text)           /* All .text sections from object files */
  }

  .data : {
    *(.data)           /* All .data sections from object files */
  }

  .bss : {
    *(.bss)            /* All .bss sections from object files */
  }

  /DISCARD/ : {
    *(.comment)
    *(.note.GNU-stack)
  }
}
