/* Память для AT32F425R8T7 */
MEMORY
{
    /* Flash: 64KB, начинаем с 16KB для загрузчика */
    FLASH (rx) : ORIGIN = 0x08004000, LENGTH = 48K  /* 64K - 16K = 48K */
    /* RAM: 20KB */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
    /* CCM RAM (если есть, для AT32F425 обычно нет) */
}

/* Размер стека */
_stack_size = 2K;

SECTIONS
{
    /* Векторная таблица */
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
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        
        /* Конструкторы/деструкторы C++ */
        KEEP(*(.init))
        KEEP(*(.fini))
        
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    /* ARM sections */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >FLASH
    
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    /* Предопределенные данные */
    .preinit_array :
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
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH

    /* Источник данных во FLASH */
    _sidata = LOADADDR(.data);

    .data : 
    {
        . = ALIGN(4);
        _sdata = .;        /* Начало в RAM */
        *(.data)
        *(.data*)
        
        . = ALIGN(4);
        _edata = .;        /* Конец в RAM */
    } >RAM AT> FLASH

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;         /* Начало .bss */
        *(.bss)
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(4);
        _ebss = .;         /* Конец .bss */
    } >RAM

    /* Стек */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _estack = .;
        . = . + _stack_size;
        . = ALIGN(8);
    } >RAM

    /* Удаляем информацию для отладки */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    /* Символы для отладки */
    .ARM.attributes 0 : { *(.ARM.attributes) }
}