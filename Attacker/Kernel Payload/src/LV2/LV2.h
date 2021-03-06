
#ifndef LV2_H
#define LV2_H

#include "../Types.h"
#include "../ELF.h"
#include "../Config.h"
#include "../LV1/LV1.h"
#include "SystemCalls.h"

#define LV2_BASE      ( 0x8000000000000000ULL )
#define LV2_HTAB_BASE ( 0x800000000F000000ULL )
#define LV2_HTAB_SIZE ( 0x40000 )
#define LV2_HV_BASE   ( LV2_BASE + 0x14000000ULL )

#define LIB_LV2_INIT_SUCCESS                ( 0x00001337UL )
#define LIB_LV2_OPD_SEG_NOT_FOUND           ( 0x0000DEADUL )
#define LIB_LV2_SYSCALL_TABLE_NOT_FOUND     ( 0x0000DEAEUL )
#define LIB_LV2_SYSCALL_DISPATCH_NOT_FOUND  ( 0x0000DEAFUL )
#define LIB_LV2_ADDRESS_NOT_FOUND           ( 0x0000DEAFUL )

struct LibLV2Context {
    //
    // Table of contents.
    //
	uint64_t KernelTOC;
	uint64_t PayloadTOC;

    //
    // Last proceedure descriptor inside the .OPD segment.
    //
    OPD_t* KernelLastOPDEntry;
    OPD_t** SystemCallTable;
    uint32_t* SystemCallDispatchBranch;

    lv1::ps3_firmware_version FirmwareVersion;

    //
    // TOC function pointers.
    //
	OPD_t* lv2_console_get_instance_opd;
	OPD_t* lv2_console_write_opd;
	OPD_t* lv2_console_write_async_opd;
    OPD_t* lv2_console_putc_opd;
    OPD_t* lv2_console_flush_opd;
    OPD_t* lv2_clprintf_opd;
    OPD_t* page_allocate_opd;
    OPD_t* page_free_opd;
    OPD_t* kmem_export_to_proc_opd;
    OPD_t* kmem_unexport_from_proc_opd;

    int (*lv2_kern_tty_write)(const char* Text, size_t Length);

	void( *printf )( const char* fmt, ... );
};

extern LibLV2Context LibLV2;

void LibLV2DebugPrint(
    const char* Format,
    ...
);

uint64_t LibLV2Init(
    uint64_t TOC,
    uint64_t PayloadTOC
);

#ifdef HAS_DEBUG_PRINTS
    #define DEBUG_PRINT(Format, ...) LibLV2DebugPrint( Format, ##__VA_ARGS__ )
#else
    #define DEBUG_PRINT(Format, ...)
#endif

#endif // !LV2_H