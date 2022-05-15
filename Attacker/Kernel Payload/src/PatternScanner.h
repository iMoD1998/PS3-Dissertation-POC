#ifndef PATTERN_SCANNER_H
#define PATTERN_SCANNER_H

#include "Types.h"
#include "FNV.h"
#include "Utils.h"

#define NO_INLINE __attribute__((noinline)) 

constexpr bool IsSkipChar(
    char Character
)
{
    return Character == '?' ||
           Character == '*';
}

class Signature 
{
public:
    uint64_t Hash;
    uint64_t Mask;

    //
    // Converts IDA style string to mask/hash combo.
    //
    constexpr static inline Signature FromString(
        const char* Pattern
    )
    {
        Signature Result        = { FNV_OFFSET_BASIS_64, 0 };
        char      LastCharacter = NULL;
        int       MaskBitIndex  = 0;

        while ( *Pattern != 0 )
        {
            const auto CurrentCharacter = Pattern[ 0 ];
            const bool HasNextCharacter = Pattern[ 1 ] != 0;

            if ( CurrentCharacter == ' ' )
            {
                LastCharacter = CurrentCharacter;
            }
            //
            // If we hit a wildcard character and the last character is not also a wildcard,
            // this must be the start of a wildcard block.
            //
            else if ( IsSkipChar( CurrentCharacter ) )
            {
                if ( !IsSkipChar( LastCharacter ) )
                {
                    MaskBitIndex++;
                }
            }
            //
            // Not a wild card check if we have a valid hex identifier.
            //
            else
            {
                if ( !HasNextCharacter )
                    return Signature{};

                const auto CurrentHexChar = Utils::HexCharValue( CurrentCharacter );
                const auto NextHexChar    = Utils::HexCharValue( Pattern[ 1 ] );

                if ( CurrentHexChar == 0xFF || NextHexChar == 0xFF )
                    return Signature{};

                uint8_t HexByte = ( CurrentHexChar << 4 ) | NextHexChar;

                Result.Hash =  FNV1A_ITERATION( HexByte, Result.Hash, FNV_PRIME_64 );
                Result.Mask |= ( 1ULL << MaskBitIndex++ );

                Pattern++;
            }

            Pattern++;

            LastCharacter = CurrentCharacter;
        }

        return Result;
    }

    inline bool Matches( 
        uint64_t Memory
    ) const
    {
        return NonCryptoHashWithMaskFNV1A64( (const char*)Memory, this->Mask ) == this->Hash;
    }

    enum CallbackStatus
    {
        SCANNER_CONTINUE,
        SCANNER_STOP
    };

    #define ALIGN_ADDR(Address, AlignAmount) uintptr_t( Address & ~( AlignAmount - 1 ) )

    template<typename T, bool IsCodeAligned, bool StopOnFirstResult>
    uint64_t FindMatchesEx(
        uint64_t          Start,
        size_t            Length,
        T&                UserData,
        CallbackStatus   (*CallbackFn)( T& UserData, uint64_t Result )
    ) const
    {
        const auto AlignSize    = IsCodeAligned ? 4 : 1;
        const auto StartAddress = ALIGN_ADDR( Start, AlignSize );
        const auto StepSize     = AlignSize;

        for( auto i = 0ULL; i < (Length - AlignSize); i += StepSize )
        {
            const auto SearchAddress = StartAddress + i;

            if( this->Matches( SearchAddress ) )
            {   
                if( CallbackFn != NULL )
                {
                    auto Status = CallbackFn( UserData, SearchAddress );

                    if( Status == CallbackStatus::SCANNER_STOP )
                    {
                        return SearchAddress;
                    }
                }

                if( StopOnFirstResult )
                    return SearchAddress;
            }
        }

        return 0;
    }

    template<typename T>
    uint64_t FindMatch(
        uint64_t          Start,
        size_t            Length,
        T&                UserData,
        CallbackStatus  (*CallbackFn)( T& UserData, uint64_t Result )
    ) const
    {
        return FindMatchesEx<T, false, true>( Start, Length, UserData, CallbackFn );
    }

    template<typename T>
    void FindMatches(
        uint64_t          Start,
        size_t            Length,
        T&                UserData,
        CallbackStatus    (*CallbackFn)( T& UserData, uint64_t Result )
    ) const
    {
        FindMatchesEx<T, false, false>( Start, Length, UserData, CallbackFn );
    }

    template<typename T>
    uint64_t FindCodeMatch(
        uint64_t          Start,
        size_t            Length,
        T&                UserData,
        CallbackStatus   (*CallbackFn)( T& UserData, uint64_t Result )
    ) const
    {
        return FindMatchesEx<T, true, true>( Start, Length, UserData, CallbackFn );
    }

    template<typename T>
    void FindCodeMatches(
        uint64_t          Start,
        size_t            Length,
        T&                UserData,
        CallbackStatus   (*CallbackFn)( T& UserData, uint64_t Result )
    ) const
    {
        FindMatchesEx<T, true, false>( Start, Length, UserData, CallbackFn );
    }

    #include "ELF.h"

    //
    // Search for a proccedure descriptor (OPD Entry) for a given function address.
    //
    NO_INLINE
    static OPD_t* FindProcedureDescriptor( 
        uint64_t Function,
        OPD_t*   LastProcedureDescriptor
    )
    {
        auto CurrentProcedureDescriptor = LastProcedureDescriptor;

        while( CurrentProcedureDescriptor->TOC == LastProcedureDescriptor->TOC )
        {
            if( CurrentProcedureDescriptor->Function == Function )
                return CurrentProcedureDescriptor;

            CurrentProcedureDescriptor--;
        }

        return 0;
    }

private:
    constexpr Signature() : Hash( 0 ), Mask( 0 ) { }
    constexpr Signature( uint64_t Hash, uint64_t Mask ) : Hash( Hash ), Mask( Mask ) { }
};

#define MAKE_PATTERN(X) \
	[]( ) -> auto { \
		constexpr auto Sig = Signature::FromString( X ); \
		\
		return Sig; \
	}( )

#endif // !PATTERN_SCANNER_H