#include "Libc.h"

extern "C" {

void* memcpy(
    void *       Destination,
    void const * Source,
    size_t       Size
)
{
    void *const Result = Destination;

    while (Size--) {
        *(char*)Destination = *(char*)Source;
        Destination = (char*)Destination + 1;
        Source = (char*)Source + 1;
    }

    return Result;
}

void* memset(
    void*  Destination,
    int    Val,
    size_t Size
)
{
    void *const Result = Destination;

    while (Size--) {
        *(char *)Destination = (char)Val;
        Destination = (char *)Destination + 1;
    }

    return Result;
}

void* memmove(
    void*       Destination,
    void const* Source,
    size_t      Size
)
{
    void * const Result = Destination;

    if (Destination <= Source || (char *)Destination >= ((char *)Source + Size)) {

        //
        // Non-Overlapping Buffers, copy from lower to higher addresses.
        //
        while (Size--) {
            *(char *)Destination = *(char *)Source;
            Destination = (char *)Destination + 1;
            Source = (char *)Source + 1;
        }

    } else {

        //
        // Overlapping Buffers, copy from higher to lower addresses.
        //
        Destination = (char *)Destination + Size - 1;
        Source = (char *)Source + Size - 1;

        while (Size--) {
            *(char *)Destination = *(char *)Source;
            Destination = (char *)Destination - 1;
            Source = (char *)Source - 1;
        }
    }

    return Result;
}

int strcmp (
    char const *String1,
    char const *String2
)
{
    int Result = 0;

    while((Result = *(unsigned char *)String1 - *(unsigned char *)String2) == 0 && *String2) {
        ++String1;
        ++String2;
    }

    //
    //  (if positive) - (if negative) makes branchless code
    //
    return ((-Result) < 0) - (Result < 0);
}

size_t strlen( 
    const char* String
)
{
    const char* EndPtr;

    for (EndPtr = String; *EndPtr; ++EndPtr)
            ;
    return (EndPtr - String);
}

char* strcat (
    char*        Destination,
    char const*  Source
)
{
    char* CopyPtr = Destination;

    //
    // Find end of Destination.
    //
    while (*CopyPtr) CopyPtr++;

    //
    // Copy Source to end of Destination.
    //
    while ((*CopyPtr++ = *Source++));

    return Destination;                 
}

char* strcpy (
    char*       Destination,
    const char* Source
)
{
    char* Start = Destination;

    while ((*Destination++ = *Source++));

    return Start;
}

char* strncat (
    char*       Destination,
    const char* Source,
    size_t      Count
)
{
    char* Start = Destination;

    while (*Destination++)
        ;
    Destination--;

    while (Count--)
    {
        if (!(*Destination++ = *Source++))
            return Start;
    }

    *Destination = '\0';

    return Start;
}

}