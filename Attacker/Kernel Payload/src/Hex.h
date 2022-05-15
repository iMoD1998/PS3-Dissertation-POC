#include "Types.h"



#define GET_NIBBLE(V, N)      ( ( V >> ( N * 4ULL ) ) & 0xFULL )
#define IS_NIBBLE_SET( V, N ) ( V & ( 0xFULL << ( N * 4ULL ) ) )

#define GET_BYTE(V, N)        ( ( V >> ( N *  8U ) ) &  UINT8_MAX )
#define GET_WORD(V, N)        ( ( V >> ( N *  8U ) ) & UINT16_MAX )
#define GET_DWORD(V, N)       ( ( V >> ( N * 32U ) ) & UINT32_MAX )
#define GET_QWORD(V, N)       ( V )

//
// Size of the buffer used in DumpHex()
//
#define DUMP_HEX_CHUNK_SIZE  0x100


static const char* HexChars      = "0123456789ABCDEF";
static const char* HexCharsLower = "0123456789abcdef";

static const char* HexCharsByte[] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF",
};

static const char* HexCharsByteLower[] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF",
};

//
// Returns the hex digit for a 4 bit value.
//
template<const bool Lowercase = false>
static inline constexpr char NibbleToHex( uint8_t Nibble )
{
	return Lowercase ? HexCharsLower[ Nibble ] : HexChars[ Nibble ];
}

constexpr static const char ASCIIPrintableCharacterMap[] = "................................"
												           " !\"#$%&\'()*+,-./0123456789:;<=>?"
												           "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
												           "`abcdefghijklmnopqrstuvwxyz{|}~."
												           "................................"
												           "................................"
												           "................................"
												           "................................";

//
// Returns only printable ASCII characters for a byte.
//
static inline constexpr char ByteToASCIIPrintable( const uint8_t Character )
{
	return ASCIIPrintableCharacterMap[ Character ];
}

//
// Returns the hex string for a byte.
//
template<bool Lowercase = false, const int Width = 2>
static inline constexpr const char* ByteToHex( const uint8_t ByteValue )
{
	static_assert( Width > 0 && Width <= 2, "Invalid width provided to ByteToHex()" );

	if ( IS_NIBBLE_SET( ByteValue, 1 ) && Width == 1 )
	{
		return Lowercase ? HexCharsByteLower[ ByteValue ] + 1 : HexCharsByte[ ByteValue ] + 1;
	}

	return Lowercase ? HexCharsByteLower[ ByteValue ] : HexCharsByte[ ByteValue ];
}

//
// Places the hex string for a uint8_t into a buffer.
//
template<const bool Lowercase = false, const int Width = 2>
inline size_t EmitByteToHex( char* Buffer, const uint8_t ByteValue )
{
	static_assert( Width > 0 && Width <= 2, "Invalid width provided to EmitByteToHex()" );

	size_t StringSize = 0U;

	if ( IS_NIBBLE_SET( ByteValue, 1 ) || Width > 1 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( ByteValue, 1 ) );

	Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( ByteValue, 0 ) );

	return StringSize;
}

//
// Places the hex string for a word into a buffer.
//
template<const bool Lowercase = false, const int Width = 4>
inline size_t EmitWordToHex( char* Buffer, const uint16_t WordValue )
{
	static_assert( Width > 0 && Width <= 4, "Invalid width provided to EmitWordToHex()" );

	size_t StringSize = 0U;

    if ( IS_NIBBLE_SET( WordValue, 3 ) || Width > 3 )
            Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( WordValue, 3 ) );

    if ( IS_NIBBLE_SET( WordValue, 2 ) || Width > 2 )
            Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( WordValue, 2 ) );

    if ( IS_NIBBLE_SET( WordValue, 1 ) || Width > 1 )
            Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( WordValue, 1 ) );

	Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( WordValue, 0 ) );

	return StringSize;
}

//
// Places the hex string for a double word into a buffer.
//
template<const bool Lowercase = false, const int Width = 8>
inline size_t EmitDwordToHex( char* Buffer, const uint32_t DoubleWordValue )
{
	static_assert( Width > 0 && Width <= 8, "Invalid width provided to EmitDwordToHex()" );

	size_t StringSize = 0U;

	if ( IS_NIBBLE_SET( DoubleWordValue, 7 ) || Width > 7 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 7 ) );

	if ( IS_NIBBLE_SET( DoubleWordValue, 6 ) || Width > 6 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 6 ) );

	if ( IS_NIBBLE_SET( DoubleWordValue, 5 ) || Width > 5 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 5 ) );

	if ( IS_NIBBLE_SET( DoubleWordValue, 4 ) || Width > 4 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 4 ) );

	if ( IS_NIBBLE_SET( DoubleWordValue, 3 ) || Width > 3 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 3 ) );

	if ( IS_NIBBLE_SET( DoubleWordValue, 2 ) || Width > 2 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 2 ) );

	if ( IS_NIBBLE_SET( DoubleWordValue, 1 ) || Width > 1 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 1 ) );

	Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( DoubleWordValue, 0 ) );

	return StringSize;
}

//
// Places the hex string for a quad word into a buffer.
//
template<const bool Lowercase = false, const int Width = 16>
inline size_t EmitQwordToHex( char* Buffer, const uint64_t QuadWordValue )
{
	static_assert( Width > 0 && Width <= 16, "Invalid width provided to EmitQwordToHex()" );

	size_t StringSize = 0U;

	if ( IS_NIBBLE_SET( QuadWordValue, 15 ) || Width > 15 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 15 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 14 ) || Width > 14 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 14 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 13 ) || Width > 13 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 13 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 12 ) || Width > 12 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 12 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 11 ) || Width > 11 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 11 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 10 ) || Width > 10 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 10 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 9 ) || Width > 9 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 9 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 8 ) || Width > 8 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 8 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 7 ) || Width > 7 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 7 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 6 ) || Width > 6 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 6 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 5 ) || Width > 5 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 5 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 4 ) || Width > 4 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 4 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 3 ) || Width > 3 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 3 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 2 ) || Width > 2 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 2 ) );

	if ( IS_NIBBLE_SET( QuadWordValue, 1 ) || Width > 1 )
		Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 1 ) );

	Buffer[ StringSize++ ] = NibbleToHex<Lowercase>( GET_NIBBLE( QuadWordValue, 0 ) );

	return StringSize;
}
 
//
// Utility function for dumping buffers or memory into a human readable format.
//
// TODO:
// Float support?
// Decimal support?
// 
//
void DumpHex( const void*  Buffer,
	          const size_t Size,
	          const size_t Columns = 16,
	          const bool   ShowAbsoluteAddress = false,
			  const bool   ShowASCII = false,
	          const size_t ByteGrouping = 1,
	          const bool   ByteSwapGroup = true	          
);