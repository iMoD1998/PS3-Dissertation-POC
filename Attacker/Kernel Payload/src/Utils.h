#ifndef UTILS_H
#define UTILS_H

#include "ELF.h"

namespace Utils {

    constexpr uint8_t HexCharValue(
        char Character
    )
    {
        Character = Character | 32; // to lower

        if ( Character >= '0' && Character <= '9' )
            return Character - '0';

        if ( Character >= 'a' && Character <= 'f' )
            return ( Character - 'a' ) + 10;

        return 0xFF;
    }

    template<typename T>
    struct ClassFnPtrCastT;

    template<typename R, typename C, typename... A>
    struct ClassFnPtrCastT< R( C::* )( A... ) >
    {
        constexpr auto operator()( C* Self, OPD_t* Address ) const 
        {
            using Fn = R(*)( C*, A... );
            return [Address, Self] ( A... Args ) -> R {
                return __builtin_bit_cast( Fn, Address )( Self, Args... );
            };
        }
    };

    template<typename T>
    constexpr ClassFnPtrCastT<T> ClassFnPtrCast;
}

#define CLASS_FN(T, Instance, Address) Utils::ClassFnPtrCast<decltype(T)>(Instance, Address)
#define STATIC_FN(T, Address) ((decltype(T))Address)

#endif // !UTILS_H