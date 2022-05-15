#ifndef LV1_CALLS_H
#define LV1_CALLS_H

#include "../Types.h"

#define HYPERCALL_0(HyperCallIndex)                                                                 \
register uint64_t R3  __asm__ ("3");                                                                \
register uint64_t R4  __asm__ ("4");                                                                \
register uint64_t R5  __asm__ ("5");                                                                \
register uint64_t R6  __asm__ ("6");                                                                \
register uint64_t R7  __asm__ ("7");                                                                \
register uint64_t R8  __asm__ ("8");                                                                \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_1(HyperCallIndex, Arg1)                                                           \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4");                                                                \
register uint64_t R5  __asm__ ("5");                                                                \
register uint64_t R6  __asm__ ("6");                                                                \
register uint64_t R7  __asm__ ("7");                                                                \
register uint64_t R8  __asm__ ("8");                                                                \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_2(HyperCallIndex, Arg1, Arg2)                                                     \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4") = (uint64_t)Arg2;                                               \
register uint64_t R5  __asm__ ("5");                                                                \
register uint64_t R6  __asm__ ("6");                                                                \
register uint64_t R7  __asm__ ("7");                                                                \
register uint64_t R8  __asm__ ("8");                                                                \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_3(HyperCallIndex, Arg1, Arg2, Arg3)                                               \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4") = (uint64_t)Arg2;                                               \
register uint64_t R5  __asm__ ("5") = (uint64_t)Arg3;                                               \
register uint64_t R6  __asm__ ("6");                                                                \
register uint64_t R7  __asm__ ("7");                                                                \
register uint64_t R8  __asm__ ("8");                                                                \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_4(HyperCallIndex, Arg1, Arg2, Arg3, Arg4)                                         \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4") = (uint64_t)Arg2;                                               \
register uint64_t R5  __asm__ ("5") = (uint64_t)Arg3;                                               \
register uint64_t R6  __asm__ ("6") = (uint64_t)Arg4;                                               \
register uint64_t R7  __asm__ ("7");                                                                \
register uint64_t R8  __asm__ ("8");                                                                \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_5(HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5)                                   \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4") = (uint64_t)Arg2;                                               \
register uint64_t R5  __asm__ ("5") = (uint64_t)Arg3;                                               \
register uint64_t R6  __asm__ ("6") = (uint64_t)Arg4;                                               \
register uint64_t R7  __asm__ ("7") = (uint64_t)Arg5;                                               \
register uint64_t R8  __asm__ ("8");                                                                \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_6(HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)                             \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4") = (uint64_t)Arg2;                                               \
register uint64_t R5  __asm__ ("5") = (uint64_t)Arg3;                                               \
register uint64_t R6  __asm__ ("6") = (uint64_t)Arg4;                                               \
register uint64_t R7  __asm__ ("7") = (uint64_t)Arg5;                                               \
register uint64_t R8  __asm__ ("8") = (uint64_t)Arg6;                                               \
register uint64_t R9  __asm__ ("9");                                                                \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_7(HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)                       \
register uint64_t R3  __asm__ ("3") = (uint64_t)Arg1;                                               \
register uint64_t R4  __asm__ ("4") = (uint64_t)Arg2;                                               \
register uint64_t R5  __asm__ ("5") = (uint64_t)Arg3;                                               \
register uint64_t R6  __asm__ ("6") = (uint64_t)Arg4;                                               \
register uint64_t R7  __asm__ ("7") = (uint64_t)Arg5;                                               \
register uint64_t R8  __asm__ ("8") = (uint64_t)Arg6;                                               \
register uint64_t R9  __asm__ ("9") = (uint64_t)Arg7;                                               \
register uint64_t R10 __asm__ ("10");                                                               \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \

#define HYPERCALL_8(HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)                 \
register uint64_t R3  __asm__ ("3")  = (uint64_t)Arg1;                                              \
register uint64_t R4  __asm__ ("4")  = (uint64_t)Arg2;                                              \
register uint64_t R5  __asm__ ("5")  = (uint64_t)Arg3;                                              \
register uint64_t R6  __asm__ ("6")  = (uint64_t)Arg4;                                              \
register uint64_t R7  __asm__ ("7")  = (uint64_t)Arg5;                                              \
register uint64_t R8  __asm__ ("8")  = (uint64_t)Arg6;                                              \
register uint64_t R9  __asm__ ("9")  = (uint64_t)Arg7;                                              \
register uint64_t R10 __asm__ ("10") = (uint64_t)Arg8;                                              \
register uint64_t R11 __asm__ ("11") = HyperCallIndex;                                              \
                                                                                                    \
__asm__ volatile ( "sc 1"                                                                           \
                 : "=r" ( R3 ), "=r" ( R4 ), "=r" ( R5 ), "=r" ( R6 ),                              \
                   "=r" ( R7 ), "=r" ( R8 ), "=r" ( R9 ), "=r" ( R10 ), "=r" ( R11 )                \
                 : "r"  ( R11 )                                                                     \
                 : "lr", "0", "12", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory" );    \


#define HYPERCALL_RETURN(NumOut) \
HyperCallResult<NumOut> Result; \
Result.StatusCode = status_code( R3 & 0xFFFFFFFF ); \
switch( NumOut )                             \
{                                            \
    case 7: Result.SetReturnValue( 6, R10 ); \
    case 6: Result.SetReturnValue( 5, R9 );  \
    case 5: Result.SetReturnValue( 4, R8 );  \
    case 4: Result.SetReturnValue( 3, R7 );  \
    case 3: Result.SetReturnValue( 2, R6 );  \
    case 2: Result.SetReturnValue( 1, R5 );  \
    case 1: Result.SetReturnValue( 0, R4 );  \
    case 0:                                  \
        break;                               \
}                                            \
return Result;\

namespace lv1 {

    enum class status_code;

    template<int NumOut>
    struct HyperCallResult
    {
        status_code  StatusCode;
        uint64_t     ReturnRegisters[ NumOut ];

        inline uint64_t GetReturnValue( int Index ) const
        {
            return ReturnRegisters[ Index ];
        }

        inline void SetReturnValue( int Index, uint64_t Value )
        {
            ReturnRegisters[ Index ] = Value;
        }
    };

    template<int HyperCallIndex, int NumIn, int NumOut>
    struct HyperCall { };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 0, NumOut>
    {
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()() const 
        {
            HYPERCALL_0( HyperCallIndex );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 1, NumOut>
    {
        template<typename T1>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1 ) const 
        {
            HYPERCALL_1( HyperCallIndex, Arg1 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 2, NumOut>
    {
        template<typename T1, typename T2>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2 ) const 
        {
            HYPERCALL_2( HyperCallIndex, Arg1, Arg2 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 3, NumOut>
    {
        template<typename T1, typename T2, typename T3>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2, T3 Arg3 ) const 
        {
            HYPERCALL_3( HyperCallIndex, Arg1, Arg2, Arg3 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 4, NumOut>
    {
        template<typename T1, typename T2, typename T3, typename T4>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4 ) const 
        {
            HYPERCALL_4( HyperCallIndex, Arg1, Arg2, Arg3, Arg4 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 5, NumOut>
    {
        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5 ) const 
        {
            HYPERCALL_5( HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 6, NumOut>
    {
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6 ) const 
        {
            HYPERCALL_6( HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 7, NumOut>
    {
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7 ) const 
        {
            HYPERCALL_7( HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 );
            HYPERCALL_RETURN( NumOut );
        }
    };

    template<int HyperCallIndex, int NumOut>
    struct HyperCall<HyperCallIndex, 8, NumOut>
    {
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        __attribute__((noinline)) 
        HyperCallResult<NumOut> operator()( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8 ) const 
        {
            HYPERCALL_8( HyperCallIndex, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 );
            HYPERCALL_RETURN( NumOut );
        }
    };
}

#endif // !LV1_CALLS_H