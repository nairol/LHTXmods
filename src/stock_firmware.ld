
/* Existing functions in the official firmware */

halt                   = 0x7DCC;
uart_init              = 0x3788;
uart_readByteBlocking  = 0x831C;
uart_writeByteBlocking = 0x8434;
uart_handler           = 0x3844;
getSerialNumber        = 0x8498;
printf                 = 0x2028;
printString            = 0xB658;
printf_hexdump         = 0x8B00;
systick_init           = 0xC768;
systick_addTimer       = 0xC7D8;
maskOpticalReceiver    = 0xCC64;
sprintf                = 0x2048;
main                   = 0xA52C;


/* Existing global variables in the official firmware */
ootx_enable = 0x100005FC;