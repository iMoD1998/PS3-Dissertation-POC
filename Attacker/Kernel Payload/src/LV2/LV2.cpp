#include "LV2.h"
#include "String.h"
#include "SystemCalls.h"
#include "../PatternScanner.h"
#include "../PPC.h"
#include "../Utils.h"

#include "../LV1/LV1Calls.h"

LibLV2Context LibLV2;

OPD_t* FindLastOPDEntry( 
	uint64_t TOC
)
{
	uint64_t  TOCSegmentStart    = TOC - 0x8000;
	uint64_t  LastEntryUnaligned = ( TOCSegmentStart - sizeof( OPD_t ) );
	uint64_t* LastEntryTOCPtr    = (uint64_t*)( LastEntryUnaligned + __builtin_offsetof( OPD_t, TOC ) );

	//
	// OPD segment is aligned by a weird value, search backwards to account for alignment.
	//
	for( uint64_t i = 0; i < 10; ++i )
	{
		if( *LastEntryTOCPtr == TOC )
		{
			//
			// We found TOC in a OPD entry, now subtract the offset of that member.
			//
			return (OPD_t*)( uint64_t( LastEntryTOCPtr ) - __builtin_offsetof( OPD_t, TOC ) );
		}

		LastEntryTOCPtr--;
	}

	return 0;
}

#ifdef HAS_DEBUG_PRINTS
void LibLV2DebugPrint(const char* Format, ...)
{
	if( LibLV2.lv2_kern_tty_write && LibLV2.lv2_clprintf_opd )
	{
		va_list ArgList;
		va_start(ArgList, Format);

		lv2::clprintf( Format, ArgList, (lv2::PrintfCallback_t)LibLV2.lv2_kern_tty_write, NULL );

		va_end(ArgList);
	}
}
#endif

bool LibLV2IsSystemCallTable( OPD_t** Address, const OPD_t* InvalidSysCallEntry )
{
	return ( Address[  0 ] == InvalidSysCallEntry &&
	         Address[  1 ] != InvalidSysCallEntry &&
		     Address[  2 ] != InvalidSysCallEntry &&
		     Address[ 16 ] == InvalidSysCallEntry &&
		     Address[ 17 ] == InvalidSysCallEntry &&
		     Address[ 20 ] == InvalidSysCallEntry &&
		     Address[ 32 ] == InvalidSysCallEntry &&
		     Address[ 33 ] == InvalidSysCallEntry &&
		     Address[ 34 ] == InvalidSysCallEntry &&
		     Address[ 36 ] == InvalidSysCallEntry &&
		     Address[ 39 ] == InvalidSysCallEntry &&
		     Address[ 40 ] == InvalidSysCallEntry &&
		     Address[ 42 ] == InvalidSysCallEntry );
}

uint64_t LibLV2Init(
	uint64_t TOC,
	uint64_t PayloadTOC
)
{
    lv1::hypercall::get_firmware_version( &LibLV2.FirmwareVersion );

    LibLV2.KernelTOC  = TOC;
	LibLV2.PayloadTOC = PayloadTOC;
	LibLV2.KernelLastOPDEntry = FindLastOPDEntry( TOC );
	LibLV2.SystemCallTable = (OPD_t**)0x800000000038a4e8;

	if( LibLV2.KernelLastOPDEntry == NULL )
		return LIB_LV2_OPD_SEG_NOT_FOUND;

	MAKE_PATTERN( "3C 60 80 01 60 63 00 03 4E 80 00 20" ).FindCodeMatches(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {

			const auto ProcDescriptor = Signature::FindProcedureDescriptor( Result, Context.KernelLastOPDEntry );

			uint64_t TOCSegmentEnd = Context.KernelTOC + 0x8000;

			//
			// Search for XREF to this systemcall not implmented proc descriptor.
			//
			for( uint64_t i = 0; i < 0x100000; i += 0x8 )
			{
				auto SearchXref = (OPD_t**)( TOCSegmentEnd + i );

				if( *SearchXref == ProcDescriptor )
				{
					if( LibLV2IsSystemCallTable( SearchXref, ProcDescriptor ) )
					{
						LibLV2.SystemCallTable = SearchXref;
						return Signature::CallbackStatus::SCANNER_STOP;
					}
				}
			}

			return Signature::CallbackStatus::SCANNER_CONTINUE;
		}
	);	

	if( LibLV2.SystemCallTable == NULL )
		return LIB_LV2_SYSCALL_TABLE_NOT_FOUND;

	MAKE_PATTERN( "7D A8 03 A6 4E 80 00 21" ).FindCodeMatch(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {

			Context.SystemCallDispatchBranch = (uint32_t*)( Result + 4 );

			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	if( LibLV2.SystemCallDispatchBranch == NULL )
		return LIB_LV2_SYSCALL_DISPATCH_NOT_FOUND;

	MAKE_PATTERN( "E8 ?? 00 10 ?? ?? ?? ?? 38 84 FF FC" ).FindCodeMatch(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			const auto PrintfBranchAddress = PPCGetBranchAddress( Result + 0xC );

			if( PrintfBranchAddress )
			{
				const auto CLPrintfBranchAddress = PPCGetBranchAddress( PrintfBranchAddress + 0x60 );

				if( CLPrintfBranchAddress )
				{
					Context.lv2_clprintf_opd = Signature::FindProcedureDescriptor( CLPrintfBranchAddress, Context.KernelLastOPDEntry );
				}

				Context.printf = STATIC_FN( Context.printf, Signature::FindProcedureDescriptor( PrintfBranchAddress, Context.KernelLastOPDEntry ) );
			}

			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "?? ?? ?? ?? 7F 84 E3 78 7F A5 EB 78 ?? ?? ?? ?? 7F 83 E3 78 7F A4 07 B4 ?? ?? ?? ??" ).FindCodeMatches(
		LibLV2.SystemCallTable[ SYS_CONSOLE_WRITE ]->Function, 0x200, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			const auto BranchAddress = PPCGetBranchAddress( Result + 0x18 );

			if( BranchAddress )
			{
				Context.lv2_kern_tty_write = (decltype(Context.lv2_kern_tty_write))Signature::FindProcedureDescriptor( BranchAddress, Context.KernelLastOPDEntry );
			}

			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "E9 22 ?? ?? 88 09 00 00 2F 80 00 00 41 9E ?? ?? E8 62" ).FindCodeMatch(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			Context.lv2_console_get_instance_opd = Signature::FindProcedureDescriptor( Result, Context.KernelLastOPDEntry );
			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "7C 1B E8 AE 3B ?? 00 01 3B ?? ?? ?? 2F 80 00 0A" ).FindCodeMatches(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			auto Offset = 0x68UL;

			if( Context.FirmwareVersion < lv1::ps3_firmware_version( 0, 9, 1 ) )
				Offset = 0x60UL;
			
			if( Context.lv2_console_write_opd == 0 )
			{
				Context.lv2_console_write_opd = Signature::FindProcedureDescriptor( Result - Offset, Context.KernelLastOPDEntry );

				return Signature::CallbackStatus::SCANNER_CONTINUE;
			}

			Context.lv2_console_write_async_opd = Signature::FindProcedureDescriptor( Result - Offset, Context.KernelLastOPDEntry );

			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "7C 80 23 78 7C A9 2B 78 7C CB 33 78 7C E8 3B 78 7C 64 1B 78 7C 05 03 78 7D 26 4B 78 7D 67 5B 78" ).FindCodeMatch(
		LV2_BASE, 0x80000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			Context.page_allocate_opd = Signature::FindProcedureDescriptor( Result, Context.KernelLastOPDEntry );
			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "7C 64 1B 78 38 A0 00 09 38 60 00 00" ).FindCodeMatch(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			const auto BranchAddress = PPCGetBranchAddress( Result + 0xC );

			if( BranchAddress )
			{
				Context.page_free_opd = Signature::FindProcedureDescriptor( BranchAddress, Context.KernelLastOPDEntry );
			}

			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "7F A0 48 00 3D 20 80 01 7C DF 33 78 2F 25 00 00" ).FindCodeMatch(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			Context.kmem_export_to_proc_opd = Signature::FindProcedureDescriptor( Result - 0x18, Context.KernelLastOPDEntry );
			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);

	MAKE_PATTERN( "E8 83 00 58 E8 63 00 68 4B ?? ?? ??" ).FindCodeMatch(
		LV2_BASE, 0x800000, LibLV2, +[] ( LibLV2Context& Context, uint64_t Result ) -> Signature::CallbackStatus {
			const auto BranchAddress = PPCGetBranchAddress( Result + 0x8 );

			if( BranchAddress )
			{
				Context.kmem_unexport_from_proc_opd = Signature::FindProcedureDescriptor( BranchAddress, Context.KernelLastOPDEntry );
			}

			return Signature::CallbackStatus::SCANNER_STOP;
		}
	);
		

	if( LibLV2.lv2_console_get_instance_opd == 0 ||
	 	LibLV2.lv2_console_write_opd        == 0 || 
	 	LibLV2.lv2_console_write_async_opd  == 0 || 
		LibLV2.printf                       == 0 )
	{
		return LIB_LV2_ADDRESS_NOT_FOUND;
	}

	DEBUG_PRINT( "lv2_console_get_instance_opd 0x%llx\n", LibLV2.lv2_console_get_instance_opd );
	DEBUG_PRINT( "lv2_console_write_opd 0x%llx\n",        LibLV2.lv2_console_write_opd );
	DEBUG_PRINT( "lv2_console_write_async_opd 0x%llx\n",  LibLV2.lv2_console_write_async_opd );
	DEBUG_PRINT( "lv2_console_putc_opd 0x%llx\n",         LibLV2.lv2_console_putc_opd );
	DEBUG_PRINT( "lv2_console_flush_opd 0x%llx\n",        LibLV2.lv2_console_flush_opd );
	DEBUG_PRINT( "lv2_clprintf_opd 0x%llx\n",             LibLV2.lv2_clprintf_opd );
	DEBUG_PRINT( "lv2_kern_tty_write 0x%llx\n",           LibLV2.lv2_kern_tty_write );
	DEBUG_PRINT( "page_allocate_opd 0x%llx\n",            LibLV2.page_allocate_opd );
	DEBUG_PRINT( "page_free_opd 0x%llx\n",                LibLV2.page_free_opd );
	DEBUG_PRINT( "kmem_export_to_proc_opd 0x%llx\n",      LibLV2.kmem_export_to_proc_opd );
	DEBUG_PRINT( "kmem_unexport_to_proc_opd 0x%llx\n",    LibLV2.kmem_unexport_from_proc_opd );

	return LIB_LV2_INIT_SUCCESS;
}