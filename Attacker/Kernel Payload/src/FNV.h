#ifndef FNV_H
#define FNV_H

#include "Types.h"

//
// FNV Parameters
//
#define FNV_OFFSET_BASIS_32 ( 0x811C9DC5UL )
#define FNV_PRIME_32        ( 0x001000193 )
#define FNV_OFFSET_BASIS_64 ( 0xCBF29CE484222325ULL )
#define FNV_PRIME_64        ( 0x00000100000001B3ULL )

//
// FNV Iterations
// B = Byte/Character
// H = CurrentHashValue
// P = Prime
// 
// FNA[0-1] = ( Hash * Prime ) ^ Byte
// FNV-1A   = ( Hash ^ Byte  ) * Prime
//
#define FNV1_ITERATION( B, H, P )  ( ( H * P ) ^ B )
#define FNV1A_ITERATION( B, H, P ) ( ( H ^ B ) * P )

//
// Generates a FNV1 hash for a given input skipping any bytes
// that also dont have bits set in the mask.
//
// Bitmask expects bits beginning from LSB.
//
// Example:
// NonCryptoHashWithMaskFNV1A("Hello World!", 0b10011) == FNV1A("Heo")
// NonCryptoHashWithMaskFNV1A("Hello World!", 0b01111) == FNV1A("Hell")
//
template<typename T, T OffsetBasis, T Prime, bool FNV1A>
__attribute__((noinline))
constexpr static T NonCryptoHashWithMaskFNV(
	const char*    Input,
	const uint64_t Mask
)
{
	if( Mask == 0 )
        return 0ULL;

	const auto MaskSize = 64ULL - __builtin_clzll( Mask );
	auto       Hash     = OffsetBasis;

	for ( auto i = 0ULL; i < MaskSize + 1; ++i )
	{
		if ( ( Mask >> i ) & 1ULL )
		{
			if( FNV1A )
				Hash = FNV1A_ITERATION( Input[ i ], Hash, Prime );
			else
				Hash = FNV1_ITERATION( Input[ i ], Hash, Prime );
		}
	}

	return Hash;
}

template<typename T, T OffsetBasis, T Prime, bool FNV1A>
constexpr static T NonCryptoHashFNV(
	const char* Input
)
{
	auto Hash = OffsetBasis;

	while( *Input != NULL )
	{
		if( FNV1A )
			Hash = FNV1A_ITERATION( *( Input++ ), Hash, Prime );
		else
			Hash = FNV1_ITERATION( *( Input++ ), Hash, Prime );
	}

	return Hash;
}

constexpr auto NonCryptoHashFNV132 = NonCryptoHashFNV<uint32_t, FNV_OFFSET_BASIS_32, FNV_PRIME_32, false>;
constexpr auto NonCryptoHashFNV164 = NonCryptoHashFNV<uint64_t, FNV_OFFSET_BASIS_32, FNV_PRIME_32, false>;

constexpr auto NonCryptoHashFNV1A32 = NonCryptoHashFNV<uint32_t, FNV_OFFSET_BASIS_32, FNV_PRIME_32, true>;
constexpr auto NonCryptoHashFNV1A64 = NonCryptoHashFNV<uint64_t, FNV_OFFSET_BASIS_64, FNV_PRIME_64, true>;

constexpr auto NonCryptoHashWithMaskFNV132  = NonCryptoHashWithMaskFNV<uint32_t, FNV_OFFSET_BASIS_32, FNV_PRIME_32, false>;
constexpr auto NonCryptoHashWithMaskFNV164  = NonCryptoHashWithMaskFNV<uint64_t, FNV_OFFSET_BASIS_64, FNV_PRIME_64, false>;

constexpr auto NonCryptoHashWithMaskFNV1A32 = NonCryptoHashWithMaskFNV<uint32_t, FNV_OFFSET_BASIS_32, FNV_PRIME_32, true>;
constexpr auto NonCryptoHashWithMaskFNV1A64 = NonCryptoHashWithMaskFNV<uint64_t, FNV_OFFSET_BASIS_64, FNV_PRIME_64, true>;

static_assert( NonCryptoHashWithMaskFNV1A64( "Hello World!", 0b10011 ) == 0x49534F19CD9C93DB, "Retard its wrong!" );
static_assert( NonCryptoHashWithMaskFNV1A64( "Hello World!", 0b01111 ) == 0x3526FDD86112AF06, "Retard its wrong!" );
static_assert( NonCryptoHashWithMaskFNV1A64( "Hello World!", 0b110111101101 ) == 0x85568AA512D10264, "Retard its wrong!" );

#endif // !FNV_H