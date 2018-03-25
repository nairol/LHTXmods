// This file contains important base functionality

#include <stdint.h>
#include <stdbool.h>
#include <stock_firmware.h>


// Functions added by the patch
void init_sram1_and_call_main( );


// Dummy functions for generating the correct branch instruction in the right place
void __attribute__((section(".HOOK_15A0"))) __attribute__((naked)) HOOK_15A0( ) { init_sram1_and_call_main(); }





void init_sram1_and_call_main( )
{
	// Enable Clocks
	uint32_t* SYSAHBCLKCTRL = 0x40048080;
	*SYSAHBCLKCTRL |= (1<<26); // Enable clock to SRAM1

	uint32_t* sram1 = 0x20000000;
	for( uint32_t i = 0; i<(0x800/4); i+=4 )
	{
		sram1[i+0] = 0;
		sram1[i+1] = 0;
		sram1[i+2] = 0;
		sram1[i+3] = 0;
	}

	main();
}
