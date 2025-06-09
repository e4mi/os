OUTPUT_FORMAT("binary");
OUTPUT_ARCH(i386);
SECTIONS {
  . = 0x7c00;
  _org = .;
  .boot : {
    *(.boot)
  }
  . = 0x7dfe;
  .sig : {
    SHORT(0xaa55);
  }
  . = 0x7e00;
  .text : {
    *(.text*)
    *(.rodata*)
    *(.data*)
    *(.bss*)
  }
  .pad : {
    . += 512 - (. % 512 + 1);
    BYTE(0);
  }
  _size = ((. - _org) + 511) / 512;
}