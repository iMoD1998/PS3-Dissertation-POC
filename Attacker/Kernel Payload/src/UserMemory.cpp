#include "LV2/LV2.h"
#include "LV2/Process.h"
#include "LV2/Memory.h"
#include "LV2/Network.h"
#include "LV2/Scheduler.h"
#include "LV2/Thread.h"

#include "UserMemory.h"

UserMappedMemory AllocateUserPage( 
    lv2::process* Process,
    size_t        Size,
    uint64_t      ExportFlags
)
{
    void*    KernelPage = NULL;
    uint64_t UserPage = NULL;

    auto Error = lv2::page_allocate( Process, Size, 0x2F, lv2::page_size::PAGE_SIZE_4K, &KernelPage );

    if( Error )
    {
        DEBUG_PRINT( "lv2::page_allocate error: %x\n", Error );
        return UserMappedMemory( 0, 0, 0 );
    }

    DEBUG_PRINT( "Allocated page at: 0x%llx\n", KernelPage );

    Error = lv2::kmem_export_to_proc( Process, KernelPage, ExportFlags, &UserPage );

    if( Error )
    {
        DEBUG_PRINT( "lv2::kmem_export_to_proc error: %x\n", Error );
        return UserMappedMemory( 0, 0, 0 );
    }

    DEBUG_PRINT( "Exported page to: 0x%llx\n", UserPage );

    return UserMappedMemory( KernelPage, UserPage, Size );
}

void FreeUserModePages( 
    lv2::process*     Process,
    UserMappedMemory& MemoryManager
)
{
    auto Error = lv2::kmem_unexport_from_proc( Process, MemoryManager.UserPage );

    DEBUG_PRINT( "Unexporting page: 0x%llx\n", MemoryManager.UserPage );

    if( Error )
    {
        DEBUG_PRINT( "lv2::kmem_unexport_from_proc error: %x\n", Error );
        return;
    }

    DEBUG_PRINT( "Freeing page: 0x%llx\n", MemoryManager.KernelPage );

    Error = lv2::page_free( Process, MemoryManager.KernelPage, 0x2F );

    if( Error )
    {
        DEBUG_PRINT( "lv2::page_free error: %x\n", Error );
    }

    DEBUG_PRINT( "Page freed: 0x%llx\n", MemoryManager.KernelPage );
}

void AllocateUserModePagesWithCallback( 
    lv2::process* Process,
    size_t        Size,
    void          (*Callback)( UserMappedMemory& Memory )
)
{
    auto MemoryManager = AllocateUserPage( Process, Size );

    Callback( MemoryManager );

    FreeUserModePages( Process, MemoryManager );
}