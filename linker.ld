OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
ENTRY(_start)
SECTIONS
{
. = ALIGN(8);
. = 0x400000;
.text : { *(.text) }

. = ALIGN(8);
/*
 * Check also these files:
 * https://github.com/kth-step/EmbExp-ProgPlatform/blob/master/board/rpi4/rpi4.ld#L19
 * https://github.com/kth-step/HolBA/blob/master/src/tools/scamv/persistence/experimentsLib.sml#L282
 */
. = 0x80200000;
.data : { *(.data) }
.bss : { *(.bss) }



	/DISCARD/ : { *(.dynsym) }
	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
}
