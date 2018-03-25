#include <stdint.h>
#include <stdbool.h>
#include <stock_firmware.h>


// Functions added by the patch
//void         __attribute__ ((section (".functions"))) myFunction( );


// Dummy functions for generating the correct branch instruction in the right place
//uint8_t __attribute__((section(".HOOK_xxxx"))) __attribute__((naked)) HOOK_xxxx( ) { return targetFunction(); }
//void    __attribute__((section(".HOOK_xxxx"))) __attribute__((naked)) HOOK_xxxx( uint8_t r0 ) { targetFunction( r0 ); }
//ASM alternative:
//void __attribute__((section(".HOOK_xxxx"))) __attribute__((naked)) HOOK_xxxx( ) { asm volatile("BL targetFunction"); }


// Functions that replace existing functions in place
// Make sure that the new function is no longer than the old one!
//bool    __attribute__((section(".FUNC_xxxx"))) FUNC_xxxx( ) { return targetFuntion( ); }
//void __attribute__((section(".FUNC_xxxx"))) __attribute__((naked)) FUNC_xxxx( ) { asm volatile("MOV R0, R4 \n\t BL targetFunction \n\t POP {R3-R7,PC}"); }


// Constants at specific addresses (e.g. function pointers)
//static void* __attribute__((section(".DATA_xxxx"))) __attribute__((used)) DATA_xxxx = myFunction + 1;


// Constants that the new functions use.
// They must be assigned to an empty FLASH block and set in the firmware file.
//static __attribute__((used)) uint32_t myConst = 0;


// Global variables added by the new functions.
// They must be assigned to an empty RAM block! E.g. 0x10001500
//uint32_t myVariable;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions added by the patch:



