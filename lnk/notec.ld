OUTPUT_ARCH(sh3)
ENTRY(initialize)
MEMORY
{
        rom  : o = 0x00300200, l = 512k
        ram  : o = 0x08100000, l = 64k  /* pretty safe guess */
}
SECTIONS
{
        .text : {
                *(.pretext)     /* init stuff */
                *(.text)
        } > rom
        .rodata : {
                *(.rodata)
                *(.rodata.*)
                _romdata = . ;  /* symbol for initialization data */
        } > rom
        .bss : {
                _bbss = . ;
                _bssdatasize = . ;
                LONG(0);        /* bssdatasize */
                *(.bss) *(COMMON);
                _ebss = . ;
        } > ram
        .data BLOCK(4) : AT(_romdata) {
                _bdata = . ;
                *(.data);
                _edata = . ;
        } > ram
}
