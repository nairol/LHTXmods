#include <stdint.h>
#include <stdbool.h>


/* This file patches some bugs in the stock firmware. None of them are critical. */


// Next line is bug fix for the command "ram". First address shown in hex dump was always 0x10000000.
void __attribute__((section(".FUNC_5594"))) __attribute__((naked)) FUNC_5594( ) { asm volatile("MOV R3, R1 \n\t BL printf_hexdump \n\t POP {R3-R7,PC}"); }

// Next 5 lines fix the superfluous type conversions in ootx_rebuild_frame.
// All fcal values were converted 3 times (float->halffloat->float->halffloat), now only once.
void __attribute__((section(".FUNC_32D8"))) __attribute__((naked)) FUNC_32D8( ) { asm volatile("NOP \n\t NOP \n\t NOP \n\t NOP"); }
void __attribute__((section(".FUNC_32F0"))) __attribute__((naked)) FUNC_32F0( ) { asm volatile("NOP \n\t NOP \n\t NOP \n\t NOP"); }
void __attribute__((section(".FUNC_3300"))) __attribute__((naked)) FUNC_3300( ) { asm volatile("NOP \n\t NOP \n\t NOP \n\t NOP"); }
void __attribute__((section(".FUNC_3316"))) __attribute__((naked)) FUNC_3316( ) { asm volatile("NOP \n\t NOP \n\t NOP \n\t NOP"); }
void __attribute__((section(".FUNC_332A"))) __attribute__((naked)) FUNC_332A( ) { asm volatile("NOP \n\t NOP \n\t NOP \n\t NOP"); }

// Next 3 lines fix the inconsistent table alignment in the auto-completion output (TAB key).
static const char __attribute__((used)) autoCompletionFormatString[] = "%-11s%s\r\n";
static char* __attribute__((section(".DATA_5EBC"))) __attribute__((used)) DATA_5EBC = autoCompletionFormatString;
void __attribute__((section(".FUNC_5C72"))) __attribute__((naked)) FUNC_5C72( ) { asm volatile("LDR R0, [PC, #0x248]"); }
