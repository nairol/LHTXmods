

/* Dummy functions for generating the correct branch instruction at the right place */
.HOOK_A568  0xA568 : {KEEP(*(.HOOK_A568))}   > FLASH
.HOOK_8502  0x8502 : {KEEP(*(.HOOK_8502))}   > FLASH
.HOOK_B652  0xB652 : {KEEP(*(.HOOK_B652))}   > FLASH
.HOOK_B65E  0xB65E : {KEEP(*(.HOOK_B65E))}   > FLASH
.HOOK_B66E  0xB66E : {KEEP(*(.HOOK_B66E))}   > FLASH
.HOOK_A5B6  0xA5B6 : {KEEP(*(.HOOK_A5B6))}   > FLASH
.HOOK_C60A  0xC60A : {KEEP(*(.HOOK_C60A))}   > FLASH

/* Functions that replace original functions in place */
.FUNC_DC84  0xDC84 : {KEEP(*(.FUNC_DC84))}   > FLASH
.FUNC_DC70  0xDC70 : {KEEP(*(.FUNC_DC70))}   > FLASH
.FUNC_5594  0x5594 : {KEEP(*(.FUNC_5594))}   > FLASH

/* Constants at specific addresses */
.DATA_0098  0x0098 : {KEEP(*(.DATA_0098))}   > FLASH
.DATA_126C  0x126C : {KEEP(*(.DATA_126C))}   > FLASH
.DATA_2024  0x2024 : {KEEP(*(.DATA_2024))}   > FLASH
.DATA_2040  0x2040 : {KEEP(*(.DATA_2040))}   > FLASH

.USB_dev_desc  : {KEEP(*(.USB_dev_desc))}  > FLASH_FREE
.USB_conf_desc : {KEEP(*(.USB_conf_desc))} > FLASH_FREE
