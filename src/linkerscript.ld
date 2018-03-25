MEMORY
{
	FLASH      (rx) : ORIGIN = 0x00000000, LENGTH = 0x00013C00
	FLASH_FREE (rx) : ORIGIN = 0x00013C00, LENGTH = 0x0000C400
	SRAM0      (rw) : ORIGIN = 0x10000000, LENGTH = 0x00002000
	SRAM1      (rw) : ORIGIN = 0x20000000, LENGTH = 0x00000800
	SRAM_USB   (rw) : ORIGIN = 0x20004000, LENGTH = 0x00000800
}

SECTIONS
{
	/* Functions and constant data added by the patch */
    .text   : {KEEP(*(.text*))}   > FLASH_FREE
    .rodata : {KEEP(*(.rodata*))} > FLASH_FREE
    .bss    : {KEEP(*(.bss*))}    > SRAM1

	INCLUDE stock_firmware.ld
	INCLUDE shared/system_setup.ld
	INCLUDE bugfixes/bugfixes.ld
	INCLUDE usb_cdc/usb_cdc.ld
	
}
