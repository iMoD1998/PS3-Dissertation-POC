#include "Types.h"

extern "C" {

    void* memcpy(
        void *       Destination,
        void const * Source,
        size_t       Size
    );

    void* memset(
        void*  Destination,
        int    Val,
        size_t Size
    );
    void* memmove(
        void*       Destination,
        void const* Source,
        size_t      Size
    );

    int strcmp (
        char const *String1,
        char const *String2
    );

    size_t strlen( 
        const char* String
    );

    char* strcpy (
        char*       Destination,
        const char* Source
    );

    char* strcat (
        char*        Destination,
        char const*  Source
    );

    char* strncat (
        char*       Destination,
        const char* Source,
        size_t      Count
    );

}