#include <stdio.h>
#include <string.h>
#include <fstream>

#include <memory>

#define PS3HEN_STAGE0_LENGTH_OFFSET  ( 0x1008B8 )
#define PS3HEN_STAGE0_START_OFFSET   ( 0x102000 )
#define PS3HEN_STAGE0_MAX_SIZE       ( 0x0D000 )

#define PS3HEN_STAGE2_LENGTH_OFFSET  ( 0x7FFF8 )
#define PS3HEN_STAGE2_START_OFFSET   ( 0x80000 )
#define PS3HEN_STAGE2_MAX_SIZE       ( 0x1FFF8 )

#define PS3HEN_SPRX_START_OFFSET	 ( 0x70000 )
#define PS3HEN_SPRX_MAX_SIZE		 ( 0x0FFF8 )

#define PS3HEN_PAYLOAD_SIZE			 ( 0x110000 )

uint64_t GetFileStreamSize( std::ifstream& Stream )
{
	const auto BackupPos = Stream.tellg( );

	Stream.seekg( 0, std::ios::beg );

	const auto Beginning = Stream.tellg( );

	Stream.seekg( 0, std::ios::end );

	const auto End = Stream.tellg( );

	const auto Size = ( End - Beginning );

	Stream.seekg( BackupPos );

	return Size;
}

void WriteUint64BE( const void* Buffer, uint64_t Value )
{
	*(uint64_t*)Buffer = _byteswap_uint64( Value );
}

int main( int ArgCount, const char* ArgValue[ ] )
{
	if ( ArgCount < 3 )
	{
		printf( "Usage: <HEN_PAYLOAD> <KERNEL_PAYLOAD> <OUT_FILE>\n" );
		return -1;
	}

	std::ifstream HenPayload( ArgValue[ 1 ], std::ios::in | std::ios::binary );
	std::ifstream KernelPayload( ArgValue[ 2 ], std::ios::in | std::ios::binary );
	std::ofstream OutFile( ArgValue[ 3 ], std::ios::out | std::ios::binary );

	if ( !HenPayload.is_open( ) )
	{
		printf( "Failed to open HEN_PAYLOAD\n" );
		return -1;
	}

	if ( !KernelPayload.is_open( ) )
	{
		printf( "Failed to open KERNEL_PAYLOAD\n" );
		return -1;
	}

	if ( !OutFile.is_open( ) )
	{
		printf( "Failed to open OUT_FILE\n" );
		return -1;
	}

	const auto KernelPayloadSize = GetFileStreamSize( KernelPayload );

	if ( KernelPayloadSize > PS3HEN_STAGE0_MAX_SIZE )
	{
		printf( "KERNEL_PAYLOAD is too big (max: %x) \n", PS3HEN_STAGE0_MAX_SIZE );
	}

	//
	// Setup out buffer for the final payload.
	//
	auto FinalPayloadBuffer = std::make_unique<char[ ]>( PS3HEN_PAYLOAD_SIZE );

	//
	// Copy stackframe payload to our buffer.
	//
	HenPayload.read( FinalPayloadBuffer.get( ), PS3HEN_PAYLOAD_SIZE );

	//
	// Write the payload and size for stage 0.
	//
	KernelPayload.read( &FinalPayloadBuffer[ PS3HEN_STAGE0_START_OFFSET ], KernelPayloadSize );
	WriteUint64BE( &FinalPayloadBuffer[ PS3HEN_STAGE0_LENGTH_OFFSET ], KernelPayloadSize );


	//
	// Write the payload and size for stage 2. (in this case we are not using it, so 0.
	//
	memset( &FinalPayloadBuffer[ PS3HEN_STAGE2_START_OFFSET ], 0xFF, PS3HEN_STAGE2_MAX_SIZE );
	WriteUint64BE( &FinalPayloadBuffer[ PS3HEN_STAGE2_LENGTH_OFFSET ], 0 );

	memset( &FinalPayloadBuffer[ PS3HEN_SPRX_START_OFFSET ], 0xFF, PS3HEN_SPRX_MAX_SIZE );

	OutFile.write( FinalPayloadBuffer.get( ), PS3HEN_PAYLOAD_SIZE );

	return 0;
}