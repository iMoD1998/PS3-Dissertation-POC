
#include "Hex.h"
#include "Libc.h"
#include "LV2/LV2.h"

void DumpHex( const void*  Buffer,
	          const size_t Size,
	          const size_t Columns,
	          const bool   ShowAbsoluteAddress,
			  const bool   ShowASCII,
	          const size_t ByteGrouping,
	          const bool   ByteSwapGroup
	          
)
{

	char   String[ DUMP_HEX_CHUNK_SIZE ];
	size_t StringIndex = 0;

	//
	// Example Line:
	// 0x12345678: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	// 
	// 0x12345678: 00000000 00000000 00000000 00000000 00 00 00 00 |................|
	//

	const auto AddressLength    = sizeof( uintptr_t ) * 2;
	const auto ColumnDataWidth  = ByteGrouping * 2;
	const auto ColumnCharWidths = ( Columns * ColumnDataWidth ) + Columns;
	const auto ASCIIDumpWidth   = ( ShowASCII ? ( 2 + Columns ) : 0 );
	const auto MaxLineWidth     = AddressLength + ColumnCharWidths + ASCIIDumpWidth;

	for ( size_t i = 0; i < Size; ++i )
	{
		if ( ( i % Columns ) == 0 )
		{
			//
			// Ensure we dont print a newline or ascii dump for first byte.
			//
			if ( i != 0 )
			{
				if ( ShowASCII )
				{
					//
					// We are currently after the last printed column so
					// finsh the line with the ASCII dump.
					//
					String[ StringIndex++ ] = '|';

					StringIndex += Columns;

					String[ StringIndex++ ] = '|';
				}

				String[ StringIndex++ ] = '\n';
			}

			//
			// Flush the buffer if we have no space left.
			// 
			// This has to be done after completing the current line as
			// the ASCII dump will be written ahed of the current string index.
			// 
			if ( StringIndex >= DUMP_HEX_CHUNK_SIZE - ( MaxLineWidth + 1 ) )
			{
				String[ StringIndex ] = NULL;

				LibLV2.lv2_kern_tty_write( String, StringIndex );

				StringIndex = 0;
			}

			//
			// Write offset or absolute address.
			//
			const auto Address = ( ShowAbsoluteAddress ? (uintptr_t)Buffer : 0 ) + i;

			String[ StringIndex++ ] = '0';
			String[ StringIndex++ ] = 'x';

			#ifdef	__LP64__
				StringIndex += EmitQwordToHex<false, 16>( &String[ StringIndex ], Address );
			#else
				StringIndex += EmitDwordToHex<false, 8>( &String[ StringIndex ], Address );
			#endif

			
			String[ StringIndex++ ] = ':';
			String[ StringIndex++ ] = ' ';
		}

		const auto CurrentByte    = ( (uint8_t*)Buffer )[ i ];
		//const auto ColumnGrouping = std::max( Size - i, ByteGrouping );

		//
		// Write column hex value.
		//
		StringIndex += EmitByteToHex<false, 2>( &String[ StringIndex ], CurrentByte );

		/*switch ( ColumnGrouping )
		{
			case 1:
				StringIndex += EmitByteToHex<false, 2>( &String[ StringIndex ], CurrentByte );
				break;
			case 2:
				StringIndex += EmitWordToHex<false, 2>( &String[ StringIndex ], ByteSwapGroup ? _byteswap_ushort( CurrentByte ) : CurrentByte );
				break;
			case 4:
				StringIndex += EmitDwordToHex<false, 2>( &String[ StringIndex ], ByteSwapGroup ? _byteswap_ulong( CurrentByte ) : CurrentByte );
				break;
			case 8:
				StringIndex += EmitQwordToHex<false, 2>( &String[ StringIndex ], ByteSwapGroup ? _byteswap_uint64( CurrentByte ) : CurrentByte );
				break;
		}*/

		String[ StringIndex++ ] = ' ';

		if ( ShowASCII )
		{
			//
			// Write the ASCII dump ahed of the current string position by calculating the
			// index relative to the just written value.
			//
			// 0x12345678: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
			//                                             ^
			//	                                          Example postion in this context.
			//
			// Therefore to calculate the position of where the ASCII dump will start is
			// simple as taking the number of columns we have to print and size of each
			// column and spaces between each column aslwell as the dump seperator '|'.
			//
			 
			const auto CurrentByteColumnIndex = ( i % Columns );
			const auto ColumnsLeftToPrint     = ( ( Columns - 1 ) - CurrentByteColumnIndex );
			const auto NumSpaceCharsLeft      =  ColumnsLeftToPrint;
			const auto NumByteCharsLeft       = ( ColumnsLeftToPrint * ColumnDataWidth ) + NumSpaceCharsLeft;
			const auto ASCIIDumpIndex         = ( StringIndex + NumByteCharsLeft + CurrentByteColumnIndex ) + 1;
										      
			String[ ASCIIDumpIndex ] = ByteToASCIIPrintable( CurrentByte );
		}
	}

	if ( ShowASCII )
	{
		//
		// If the size of the dump was not a multiple of the number of columns
		// we may still have residual columns to print.
		//
		const auto ResidualData    = Size % Columns;
		const auto ResidualColumns = Columns - ResidualData;
		const auto Pad             = ( ResidualColumns * ColumnDataWidth ) + ResidualColumns;

		//
		// Pad with spaces from the last printed byte to the start of the ASCII dump.
		// 
		// 0x12345678: 00 00 00 00 00 00 00 00 00 00 00                |...........     |
		//                                              ^
		//										     Padding this with spaces
		if ( ResidualData != 0 )
		{
			memset( &String[ StringIndex ], ' ', Pad );

			StringIndex += Pad;
		}

		String[ StringIndex++ ] = '|';


		//
		// Pad with spaces for the missing ASCII chars in the ASCII dump.
		// 
		// 0x12345678: 00 00 00 00 00 00 00 00 00 00 00                |...........     |
		//                                                                          ^
		//		                                                                  Padding this with spaces
		//
		if ( ResidualData != 0 )
		{
			memset( &String[ StringIndex + ResidualData ], ' ', ResidualColumns );
		}

		StringIndex += Columns;

		String[ StringIndex++ ] = '|';
	}

	//
	// Write any remaining lines.
	//
	if ( StringIndex != 0 )
	{
		String[ StringIndex++ ] = '\n';
		String[ StringIndex ] = NULL;

		LibLV2.lv2_kern_tty_write( String, StringIndex );
	}
}
