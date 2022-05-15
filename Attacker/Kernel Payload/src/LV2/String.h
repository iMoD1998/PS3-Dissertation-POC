#ifndef LV2_STRING_H
#define LV2_STRING_H

#include "../Types.h"
#include "../Utils.h"

namespace lv2 {

    typedef int(*PrintfCallback_t)(const char* String, size_t Length, void* Arg);

    static uint64_t clprintf(const char* Format, va_list Arguments, PrintfCallback_t PrintfCallback, void* Argument) 
    {
        return STATIC_FN( &clprintf, LibLV2.lv2_clprintf_opd )( Format, Arguments, PrintfCallback, Argument );
    }
}

#endif // !LV2_STRING_H