#include "../Types.h"

namespace lv2 {
    class process;
    class pu_scheduler;

    class pu_thread {
    public:
        char     _0x00[ 0x80 ];
        process* MyProcess;
    };

    class hw_thread {
    public:
        char          _0x00[ 0x48 ];
        pu_scheduler* Scheduler;
        char          _0x50[ 0x68 - 0x50 ];
        pu_thread**   Threads;
        char          _0x70[ 0xB0 - 0x70 ];
        process*      Unknown;
        char          _0xB8[ 0xC0 - 0xB8 ];
        uint16_t      SomeStatus;
    };

    static inline process* ht_current_process()
    {
        const auto HardwareThread = (hw_thread*)__mfsprg0();

        if(HardwareThread->SomeStatus == 3)
            return HardwareThread->Unknown;

        const auto PuThread = HardwareThread->Threads[0];

        if( PuThread != NULL )
            return PuThread->MyProcess;

        return NULL;
    }

    static inline process* get_current_process()
    {
        lv2::pu_scheduler::pu_sched_stop_schedule();

        auto Process = lv2::ht_current_process();

        lv2::pu_scheduler::pu_sched_cont_schedule();

        return Process;
    }

    static inline void sys_timer_usleep( usecond_t Microseconds )
    {
        SYSCALL_FN( &sys_timer_usleep, SYS_TIMER_USLEEP );
    }
}