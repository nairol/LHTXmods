#include <stdint.h>
#include <stdbool.h>
#include <stock_firmware.h>
#include <usb_cdc/app_usbd_cfg.h>

#include "usbd/usbd_rom_api.h"
#include "chip.h"


// Functions added by the patch
void __attribute__ ((section (".functions"))) onSyncFlashDone( bool opticalReceiverMaskEnable );


// Dummy functions for generating the correct branch instruction in the right place
void    __attribute__((section(".HOOK_C60A"))) __attribute__((naked)) HOOK_C60A( bool r0 ) { onSyncFlashDone( r0 ); }



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions added by the patch:

void onSyncFlashDone( bool opticalReceiverMaskEnable )
{
	maskOpticalReceiver( opticalReceiverMaskEnable );

	// my code...
}

