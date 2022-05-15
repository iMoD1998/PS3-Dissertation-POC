#ifndef LV2_PROCESS_H
#define LV2_PROCESS_H

#include "../Types.h"

namespace lv2 {
    class process {
    public:
        void*       SystemCallTable; // 0x00
        char        _0x08[0x28 - 0x8];
        uint32_t    ProcessID; // 0x28
        int         Status;    // 0x2C
        char        _0x30[0xB0 - 0x30];
        process*    ParentProcess; // 0xB0
        process*    ChildProcess; // 0xB8
        process*    NextSiblingProcess; // 0xC0
        uint64_t    NumChildren; // 0xC8
        char        _0xD0[0x160 - 0xD0];
        const char* ImageName;
    };
}

#endif // !LV2_PROCESS_H