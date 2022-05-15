
#include "../Types.h"
#include "../Utils.h"

namespace lv2 {
    class process;

    enum page_size {
        PAGE_SIZE_4K = 0x100,
        PAGE_SIZE_64K = 0x200,
        PAGE_SIZE_1M = 0x300
    };

    static inline uint32_t page_allocate( 
        lv2::process* Process,
        uint64_t      NumBytes,
        uint64_t      PageID,
        page_size     PageSize,
        void**        PageAddressOut 
    )
    {
        return STATIC_FN( &page_allocate, LibLV2.page_allocate_opd )( Process, NumBytes, PageID, PageSize, PageAddressOut );
    }

    static inline uint32_t page_free( 
        lv2::process* Process,
        void*         PageAddress,
        uint64_t      PageID
    )
    {
        return STATIC_FN( &page_free, LibLV2.page_free_opd )( Process, PageAddress, PageID );
    }

    static inline uint32_t kmem_export_to_proc(
        lv2::process* Process,
        void*         PageAddress,
        uint64_t      Flags,
        uint64_t*     PageVirtualAddressOut
    )
    {
        return STATIC_FN( &kmem_export_to_proc, LibLV2.kmem_export_to_proc_opd )( Process, PageAddress, Flags, PageVirtualAddressOut );
    }

    static inline uint32_t kmem_unexport_from_proc(
        lv2::process* Process,
        uint64_t      PageVirtualAddress
    )
    {
        return STATIC_FN( &kmem_unexport_from_proc, LibLV2.kmem_unexport_from_proc_opd )( Process, PageVirtualAddress );
    }
}