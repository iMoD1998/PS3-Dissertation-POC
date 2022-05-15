#include "LV2/LV2.h"

#include "Hooks.h"
#include "PatternScanner.h"
#include "PPC.h"
#include "SpinLock.h"
#include "Utils.h"

#include "LV2/Scheduler.h"
#include "LV2/Process.h"
#include "LV2/Thread.h"

#include "Exploit.h"

extern "C"
uint64_t PayloadEntry(
	uint64_t TOC,
	uint64_t PayloadTOC
)
{	
	if( const auto Error = LibLV2Init( TOC, PayloadTOC ) != LIB_LV2_INIT_SUCCESS )
	 	return LIB_LV2_INIT_SUCCESS;

	#ifdef SYS_CALLS_HOOK
		InstallHooks();
	#endif

	DEBUG_PRINT( "Payload Loaded Firmware: %u.%u.%u\n", LibLV2.FirmwareVersion.Major, LibLV2.FirmwareVersion.Minor, LibLV2.FirmwareVersion.Revision );
	DEBUG_PRINT( "RTOC: 0x%llx KRTOC: 0x%llx\n", LibLV2.PayloadTOC, LibLV2.KernelTOC );
	DEBUG_PRINT( "System Call Table: 0x%llx\n", LibLV2.SystemCallTable );

	#ifdef PS3_HEN
		//
		// Browser checks for successful run by looking for these bytes.
		//
		*(uint32_t *)0x8a000000=0x53434500;

		lv2::sys_timer_usleep( 500000 );
	#endif

	//DoExploit( lv2::get_current_process() );

	return 0;
}