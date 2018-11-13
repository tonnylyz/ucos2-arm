OUTPUT_ARCH(arm)
ENTRY(start)

SECTIONS
{
	. = 0x82000000;
	_image_start = .;
	. = ALIGN(4);
	.text	:
	{
        boot/start.o (.text)
        . = ALIGN(4);
		*(.text)
	}
	. = ALIGN(4);
	.rodata : { *(.rodata) }
	. = ALIGN(4);
	.data :   { 
		*(.data) 
		. = ALIGN(4);
		bss_start = .;
		*(.bss) 
		. = ALIGN(4);
		bss_end = .;
	}

  _magic_sig = 0x016f2818;
  _magic_start = _image_start;
  _magic_end = _edata;


		_edata = .;
}
