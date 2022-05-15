#include "Hooks.h"
#include "SpinLock.h"
#include "PPC.h"
#include "Libc.h"

#include "LV2/LV2.h"
#include "LV2/Process.h"
#include "LV2/Memory.h"
#include "LV2/Network.h"
#include "LV2/Scheduler.h"
#include "LV2/Thread.h"

#include "Exploit.h"
#include "UserMemory.h"

#include "LV2/PRX.h"

#include "Hex.h"

volatile int SystemCallLock;

extern "C" void SystemCallHookPrepareDispatch();

void InstallHooks()
{
    SpinLockInit( &SystemCallLock );
    PPCWriteBranchRelative( LibLV2.SystemCallDispatchBranch, ( (OPD_t*)SystemCallHookPrepareDispatch )->Function, true );
}

int sys_prx_unload_module(
    lv2::sys_prx_id_t id,
    lv2::sys_prx_flags_t flags, 
    lv2::sys_prx_unload_module_option_t* pOpt
);

void Nop()
{

}

extern "C" 
void SystemCallHookProcess( SystemCallContext* CallContext )
{
    if( CallContext->Index == _SYS_PRX_UNLOAD_MODULE )
    {
        lv2::process* Process = lv2::get_current_process();

        if( Process != NULL )
        {
            if( Process->ImageName != NULL )
            {
                //
                // Check we are coming from the VSH.
                //
                if( NonCryptoHashFNV1A64( Process->ImageName ) == NonCryptoHashFNV1A64( "/dev_flash/vsh/module/vsh.self" ) )
                {
                    auto MemoryManager = AllocateUserPage( Process, 0x2000 );

                    if( MemoryManager.KernelPage == NULL )
                        return;

                    const auto SPRXModuleInfoOpt = MemoryManager.Get<lv2::sys_prx_module_info_option_t>( 1 );
                    const auto SPRXModuleInfo    = MemoryManager.Get<lv2::sys_prx_module_info_t>( 1 );
                    const auto SPRXName          = MemoryManager.Get<char>( 256 );

                    SPRXModuleInfoOpt->Size         = 0x10;
                    SPRXModuleInfoOpt->ModuleInfo32 = SPRXModuleInfo.UserAs<uint32_t>();

                    SPRXModuleInfo->Size         = sizeof(lv2::sys_prx_module_info_t);
                    SPRXModuleInfo->FileName     = SPRXName.UserAs<uint32_t>();
                    SPRXModuleInfo->FileNameSize = 256;

                    auto sys_prx_get_module_info =  SYSCALL_F(int(*)( lv2::sys_prx_id_t id, lv2::sys_prx_flags_t flags, lv2::sys_prx_module_info_option_t *pInfo ), _SYS_PRX_GET_MODULE_INFO );

                    auto Error = sys_prx_get_module_info( CallContext->Args[ 0 ], 0, SPRXModuleInfoOpt.User() );

                    DEBUG_PRINT( "sys_prx_unload_module: %x %s\n", Error, SPRXModuleInfo->Name );


                    //
                    // Allow web browser to unload before triggering the exploit.
                    // This fixes crashes, most likely due to the sys_prx_unload_module calling extend_kstack.
                    // 
                    STATIC_FN( &sys_prx_unload_module, &CallContext->Handler )( CallContext->Args[ 0 ], CallContext->Args[ 1 ], (lv2::sys_prx_unload_module_option_t*)CallContext->Args[ 2 ] );

                    CallContext->Handler.Function = ((OPD_t*)Nop)->Function;

                    //
                    // Run exploit on browser exit.
                    //
                    if( NonCryptoHashFNV1A64( SPRXModuleInfo->Name ) == NonCryptoHashFNV1A64( "webrender_plugin_module" ) )
                    {
                        DoExploit( CallContext, Process );
                    }

                    FreeUserModePages( Process, MemoryManager );

                    DEBUG_PRINT( "Nice Poop\n");
                }
            }
        }
    }
}