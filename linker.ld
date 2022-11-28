OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
ENTRY(_start)
SECTIONS
{
. = ALIGN(8);
. = 0x400000;
.text : { *(.text) }

. = ALIGN(8);
. = 0x80100000;
.data : { *(.data) }
.bss : { *(.bss) }



	/DISCARD/ : { *(.dynsym) }
	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
}
