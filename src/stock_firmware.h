#pragma once

#include <stdint.h>
#include <stdbool.h>

// Existing functions in the official firmware.
// Assign these symbols the correct addresses in the linker script!
void     halt( const char *message, const char *function, const char *file, uint32_t line );
void     uart_init( );
uint8_t  uart_readByteBlocking( );
void     uart_writeByteBlocking( uint8_t data );
void     uart_handler( );
uint32_t getSerialNumber( );
int      printf( const char* formatString, ... );
void     printString( char* string );
void     printf_hexdump( uint32_t UNUSED_ARG, void* address, uint32_t length, uint32_t startOffset, const char* prefixString ); // Pass in 0x100003B8 for UNUSED_ARG
void     systick_init( );
int      systick_addTimer( void *functionPtr, uint32_t functionArg, bool triggerOnce, uint32_t timerValue );
void     maskOpticalReceiver( bool maskEnable );
int      sprintf( const char* str, const char* format, ... );
void     main( );

/*
void DPLL_PIDcontroller( uint32_t rotor, int32_t measuredError );
void DPLL_edge( uint32_t rotor, uint32_t edge, uint32_t timestamp, int32_t timebase_offset );
bool LTC6904_setFrequency( bool I2C_addressBit, uint32_t controlBits, uint32_t frequency );
int32_t DPLL_getRotorControlErrorValue( uint32_t rotor );
uint32_t DPLL_getRotorPIDoutputPWMvalue( uint32_t rotor );
void SystemCoreClockUpdate( );
void waitLoop( uint32_t loopCount );
*/



// Existing global variables in the official firmware.
// Assign these symbols the correct addresses!
//extern uint8_t ootx_enable;
