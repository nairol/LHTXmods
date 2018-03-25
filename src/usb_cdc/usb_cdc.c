#include <stdint.h>
#include <stdbool.h>
#include <stock_firmware.h>
#include <usb_cdc/app_usbd_cfg.h>

#include "usbd/usbd_rom_api.h"
#include "chip.h"


// Functions added by the patch
void         USB_ISR( );
void         initializeUARTandUSB( );
ErrorCode_t  CDC_BulkIn_Handler( USBD_HANDLE_T usb, void* data, uint32_t event );
ErrorCode_t  CDC_BulkOut_Handler( USBD_HANDLE_T usb, void* data, uint32_t event );
ErrorCode_t  CDC_SetLineCode_Handler( USBD_HANDLE_T hCDC, CDC_LINE_CODING* line_coding );
ErrorCode_t  CDC_SetControlLineState_Handler( USBD_HANDLE_T hCDC, uint16_t state );
void         writeByteUARTorUSB( uint8_t data );
uint8_t      readByteUARTorUSB( );
bool         checkIfByteReceivedUARTandUSB( );
void         tx_SendBuffer( );
void         printHeader( );
void         systick_init_and_add_CDC_timer( );
void         CDC_timer_callback( );


// Dummy functions for generating the correct branch instruction at the right place
uint8_t __attribute__((section(".HOOK_8502"))) __attribute__((naked)) HOOK_8502( ) { return readByteUARTorUSB(); }
void    __attribute__((section(".HOOK_A568"))) __attribute__((naked)) HOOK_A568( ) { initializeUARTandUSB(); }
void    __attribute__((section(".HOOK_B652"))) __attribute__((naked)) HOOK_B652( uint8_t r0 ) { writeByteUARTorUSB( r0 ); }
void    __attribute__((section(".HOOK_B65E"))) __attribute__((naked)) HOOK_B65E( uint8_t r0 ) { writeByteUARTorUSB( r0 ); }
void    __attribute__((section(".HOOK_B66E"))) __attribute__((naked)) HOOK_B66E( uint8_t r0 ) { writeByteUARTorUSB( r0 ); }
void    __attribute__((section(".HOOK_A5B6"))) __attribute__((naked)) HOOK_A5B6( ) { systick_init_and_add_CDC_timer(); }


// Functions that replace existing functions in place
// Make sure that the new function is no longer than the old one!
bool    __attribute__((section(".FUNC_DC84"))) FUNC_DC84( ) { return checkIfByteReceivedUARTandUSB( ); }
uint8_t __attribute__((section(".FUNC_DC70"))) FUNC_DC70( ) { return readByteUARTorUSB( ); }


// Constants at specific addresses
static void* __attribute__((section(".DATA_0098"))) __attribute__((used)) DATA_0098 = USB_ISR + 1; // USB ISR in ISR table (IRQ22 @ 0x0098)
static void* __attribute__((section(".DATA_126C"))) __attribute__((used)) DATA_126C = readByteUARTorUSB + 1;
static void* __attribute__((section(".DATA_2024"))) __attribute__((used)) DATA_2024 = writeByteUARTorUSB + 1;
static void* __attribute__((section(".DATA_2040"))) __attribute__((used)) DATA_2040 = writeByteUARTorUSB + 1;


// Global variables added by the new functions.
const USBD_API_T* g_pUsbApi;
USBD_HANDLE_T     usb;
bool              cdcConnected;
volatile uint32_t rx_read;
volatile uint32_t rx_write;
volatile uint32_t tx_read;
volatile uint32_t tx_write;
uint8_t           rxbuf[64];
uint8_t           txbuf[64];
volatile bool     tx_busy;
bool              headerShown;
bool              revertAndBackupInfoShown;;
uint32_t          USBRAM_nextFreeAddress;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions added by the patch:

void initializeUARTandUSB( )
{
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	// Enable Power to USB and USB PLL
	// ... is already done by original firmware at 0x1F7A and 0x1F82
	// register PDRUNCFG bit 10 and 8 set to 0

	// Set USB clock source and PLL divider
	// ... is already done by original firmware at 0x1F84 .. 0x1FA0
	// register USBPLLCLKSEL = 1 (USB PLL clock source is system oscillator, 12MHz)
	// register USBPLLCTRL = 0x23 (Clock multiplied by 4, output is 48MHz)
	// register USBCLKSEL = 0 (USB clock source is USB PLL)
	// register USBCLKDIV = 1 (USB clock divider is 1)

	// Enable Clocks
	uint32_t* SYSAHBCLKCTRL = 0x40048080;
	*SYSAHBCLKCTRL |= (1<<14); // Enable clock to USB
	*SYSAHBCLKCTRL |= (1<<27); // Enable clock to USB RAM

	// Configure Pins
	uint32_t* PIO0_6 = 0x40044018;
	*PIO0_6 = 0x00000092; // Use PIO0_6 as !USB_CONNECT (to connect 1.5k resistor)

	// Initialize NXP's builtin USB device stack
	USBD_API_INIT_PARAM_T usbInitParams;
	for( int i = 0; i < sizeof(USBD_API_INIT_PARAM_T); i++ )
	{
		uint8_t* data = (uint8_t *) &usbInitParams;
		data[i] = 0;
	}

	const uint32_t maxUSBStringDescriptorsSize = 128;

	usbInitParams.usb_reg_base = LPC_USB0_BASE;
	usbInitParams.max_num_ep = 3 + 1; // "+ 1" is a bug workaround...
	usbInitParams.mem_base = USB_STACK_MEM_BASE;
	usbInitParams.mem_size = USB_STACK_MEM_SIZE - maxUSBStringDescriptorsSize;


	uint8_t* USBStringDescriptors = USB_STACK_MEM_BASE + usbInitParams.mem_size;
	generateUSBStringDescriptors(USBStringDescriptors, maxUSBStringDescriptorsSize);

	USB_CORE_DESCS_T usbDescriptors;
	usbDescriptors.device_desc = &USB_DeviceDescriptor[0];
	usbDescriptors.string_desc = &USBStringDescriptors[0];
	usbDescriptors.high_speed_desc = &USB_FsConfigDescriptor[0];
	usbDescriptors.full_speed_desc = &USB_FsConfigDescriptor[0];
	usbDescriptors.device_qualifier = 0;

	ErrorCode_t err = ERR_FAILED;

	err = USBD_API->hw->Init(&usb, &usbDescriptors, &usbInitParams);
	if( err != LPC_OK )
	{
		halt("USBD init", "", "", 0);
	}

	// Initialize CDC
	USBD_HANDLE_T cdc;

	USBD_CDC_INIT_PARAM_T cdcInitParams;
	for( int i = 0; i < sizeof(USBD_CDC_INIT_PARAM_T); i++ )
	{
		uint8_t* data = (uint8_t *) &cdcInitParams;
		data[i] = 0;
	}

	cdcInitParams.mem_base = USB_STACK_MEM_BASE + (USB_STACK_MEM_SIZE - usbInitParams.mem_size);
	cdcInitParams.mem_size = usbInitParams.mem_size;
	cdcInitParams.cif_intf_desc = &usbDescriptors.high_speed_desc[0x11];
	cdcInitParams.dif_intf_desc = &usbDescriptors.high_speed_desc[0x34];
	cdcInitParams.SetCtrlLineState = CDC_SetControlLineState_Handler;

	// Only needed if MCU is used as a bridge to a real serial interface:
	//cdcInitParams.SetLineCode = CDC_SetLineCode_Handler;

	err = USBD_API->cdc->init( usb, &cdcInitParams, &cdc );

	if( err != LPC_OK )
	{
		halt("CDC init", "", "", 0);
	}

	USBRAM_nextFreeAddress = cdcInitParams.mem_base;
	// The USB CDC stack typically takes up 0x4C8 bytes.

	uint32_t cdc_in_index  = (((USB_CDC_IN_EP & 0x0F) << 1) + 1);
	uint32_t cdc_out_index = ((USB_CDC_OUT_EP & 0x0F) << 1);

	err = USBD_API->core->RegisterEpHandler(usb, cdc_in_index, CDC_BulkIn_Handler, 0);
	if( err != LPC_OK )
	{
		halt("CDC IN", "", "", 0);
	}

	err = USBD_API->core->RegisterEpHandler(usb, cdc_out_index, CDC_BulkOut_Handler, 0);
	if( err != LPC_OK )
	{
		halt("CDC OUT", "", "", 0);
	}

	cdcConnected = false;
	rx_read = 0;
	rx_write = 0;
	tx_read = 0;
	tx_write = 0;

	tx_busy = false;
	headerShown = false;
	revertAndBackupInfoShown = false;

	// Enable USB interrupts
	uint32_t* ISER = 0xE000E100;
	*ISER = (1<<22);

	// Connect to host
	USBD_API->hw->Connect(usb, 1);

	// Original function to initialize UART
	uart_init();
}

void systick_init_and_add_CDC_timer( )
{
	systick_init( );

	systick_addTimer( CDC_timer_callback, 0, false, 50 );
}

void USB_ISR() // Needs to be put in ISR table (IRQ22 @ 0x0098)! Add 1 to the function address because of Thumb instruction mode!
{
	// Bug workaround...
	uint32_t *addr = (uint32_t *) LPC_USB->EPLISTSTART;
	if ( LPC_USB->DEVCMDSTAT & _BIT(8) ) {	/* if setup packet is received */
		addr[0] &= ~(_BIT(29));	/* clear EP0_OUT stall */
		addr[2] &= ~(_BIT(29));	/* clear EP0_IN stall */
	}

	USBD_API->hw->ISR(usb);
}

void CDC_timer_callback( )
{
	if( cdcConnected )
	{
		if( !headerShown )
		{
			printHeader();
			headerShown = true;
		}

		if( (tx_read < tx_write) && !tx_busy)
		{
			tx_SendBuffer();
		}
	}
}

void printHeader( )
{
	printf( (const char*)0x37E0, 2 );
	printf( (const char*)0x37E8, 1, 1 );
	printf( (const char*)0x3828, getSerialNumber(), 436, (char*)0x3814, (char*)0x3804, (char*)0x37F4 );
	printf( "Hacked firmware: USBCDC, BUGFIXES [v4]        https://github.com/nairol/LHTXmods\r\n" );
	printString( (char*)0xDE1C );
	if( !revertAndBackupInfoShown )
	{
		printf( "Use the command \"factory load-cal\" to revert to factory config and calibration.\r\n" );
		printf( "The command \"eeprom r 0 4032\" can be used to create a backup copy of this data.\r\n" );
		revertAndBackupInfoShown = true;
	}
}

ErrorCode_t CDC_BulkIn_Handler( USBD_HANDLE_T usb, void* data, uint32_t event )
{
	tx_busy = false;

	return LPC_OK;
}

ErrorCode_t CDC_BulkOut_Handler( USBD_HANDLE_T usb, void* data, uint32_t event )
{
	if( !cdcConnected ) return LPC_OK;

	if( event == USB_EVT_OUT )
	{
		int32_t rx_readable = rx_write - rx_read;

		if( rx_readable > 0 )
		{
			return ERR_USBD_UNHANDLED;
		}
		else
		{
			// Disable USB interrupts
			uint32_t* ICER = 0xE000E180;
			*ICER = (1<<22);

			uint32_t received = USBD_API->hw->ReadEP( usb, USB_CDC_OUT_EP, &rxbuf[0] );
			rx_write = received;
			rx_read = 0;

			// Enable USB interrupts
			uint32_t* ISER = 0xE000E100;
			*ISER = (1<<22);
		}
	}
	return LPC_OK;
}

void tx_SendBuffer( )
{
	// Disable USB interrupts
	uint32_t* ICER = 0xE000E180;
	*ICER = (1<<22);

	USBD_API->hw->WriteEP( usb, USB_CDC_IN_EP, &txbuf[tx_read], (tx_write - tx_read) );
	tx_read = 0;
	tx_write = 0;
	tx_busy = true;

	// Enable USB interrupts
	uint32_t* ISER = 0xE000E100;
	*ISER = (1<<22);
}

ErrorCode_t CDC_SetControlLineState_Handler( USBD_HANDLE_T hCDC, uint16_t state )
{
	cdcConnected = state & 0x0001;

	tx_read = 0;
	tx_write = 0;
	rx_read = 0;
	rx_write = 0;

	tx_busy = false;

	headerShown = false;

	return LPC_OK;
}

void writeByteUARTorUSB( uint8_t data )
{
	if( cdcConnected )
	{

		if( tx_write == 64 )
		{
			while( tx_busy ) { }
			tx_SendBuffer();
		}

		txbuf[tx_write] = data;
		tx_write++;
	}
	else
	{
		uart_writeByteBlocking( data );
	}
}

uint8_t readByteUARTorUSB( )
{
	if( cdcConnected )
	{
		while( rx_write <= rx_read )
		{
			// waiting for RX data...
		}

		uint8_t data = rxbuf[rx_read];
		rx_read++;
		return data;
	}
	else
	{
		return uart_readByteBlocking();
	}
}


// Called periodically by the original firmware
bool checkIfByteReceivedUARTandUSB( )
{
	int32_t rx_readable = rx_write - rx_read;

	uint32_t* UART_LSR = 0x40008014;
	bool uart_byteAvailable = (((*UART_LSR)<<31)>>31)&1;

	/*
	// Forwards the commands coming from the BT chip to the PC:
	if( cdcConnected && uart_byteAvailable )
	{
		writeByteUARTorUSB( uart_readByteBlocking() );
	}
	*/

	return cdcConnected ? (rx_readable > 0) :  uart_byteAvailable ;
}
