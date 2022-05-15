#ifndef POWER_PC_H
#define POWER_PC_H

#include "Types.h"

#define MASK_N_BITS(N) ( ( 1 << ( N ) ) - 1 )

#define POWERPC_HI(X) ( ( X >> 16 ) & 0xFFFF )
#define POWERPC_LO(X) ( X & 0xFFFF )

//
// PowerPC most significant bit is addressed as bit 0 in documentation.
//
#define POWERPC_BIT32(N) ( 31 - N )

//
// Opcode is bits 0-5. 
// Allowing for op codes ranging from 0-63.
//
#define POWERPC_OPCODE(OP)       ( OP << 26 )
#define POWERPC_OPCODE_ADDI      POWERPC_OPCODE( 14 )
#define POWERPC_OPCODE_ADDIS     POWERPC_OPCODE( 15 )
#define POWERPC_OPCODE_BC        POWERPC_OPCODE( 16 )
#define POWERPC_OPCODE_B         POWERPC_OPCODE( 18 )
#define POWERPC_OPCODE_BCCTR     POWERPC_OPCODE( 19 )
#define POWERPC_OPCODE_ORI       POWERPC_OPCODE( 24 )
#define POWERPC_OPCODE_EXTENDED  POWERPC_OPCODE( 31 ) // Use extended opcodes.
#define POWERPC_OPCODE_STW       POWERPC_OPCODE( 36 )
#define POWERPC_OPCODE_LWZ       POWERPC_OPCODE( 32 )
#define POWERPC_OPCODE_LD        POWERPC_OPCODE( 58 )
#define POWERPC_OPCODE_STD       POWERPC_OPCODE( 62 )
#define POWERPC_OPCODE_MASK      POWERPC_OPCODE( 63 )

#define POWERPC_EXOPCODE(OP)     ( OP << 1 )
#define POWERPC_EXOPCODE_BCCTR   POWERPC_EXOPCODE( 528 )
#define POWERPC_EXOPCODE_MTSPR   POWERPC_EXOPCODE( 467 )

#define POWERPC_ADDI(rD, rA, SIMM)  (uint32_t)( POWERPC_OPCODE_ADDI | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM )
#define POWERPC_ADDIS(rD, rA, SIMM) (uint32_t)( POWERPC_OPCODE_ADDIS | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM )
#define POWERPC_B(LI, AA, LK)       (uint32_t)( POWERPC_OPCODE_B | ( LI & ( MASK_N_BITS( 24 ) << 2 ) ) | ( ( AA & 1 ) << 1 ) | ( LK & 1 ) )
#define POWERPC_BCCTR(BO, BI, LK)   (uint32_t)( POWERPC_OPCODE_BCCTR | ( BO << POWERPC_BIT32( 10 ) ) | ( BI << POWERPC_BIT32( 15 ) ) | ( LK & 1 ) | POWERPC_EXOPCODE_BCCTR )
#define POWERPC_LIS(rD, SIMM)       POWERPC_ADDIS( rD, 0, SIMM ) // Mnemonic for addis %rD, 0, SIMM
#define POWERPC_LI(rD, SIMM)        POWERPC_ADDI( rD, 0, SIMM )  // Mnemonic for addi %rD, 0, SIMM
#define POWERPC_MTSPR(SPR, rS)      (uint32_t)( POWERPC_OPCODE_EXTENDED | ( rS << POWERPC_BIT32( 10 ) ) | ( POWERPC_SPR( SPR ) << POWERPC_BIT32( 20 ) ) | POWERPC_EXOPCODE_MTSPR )
#define POWERPC_MTCTR(rS)           POWERPC_MTSPR( 9, rS ) // Mnemonic for mtspr 9, rS
#define POWERPC_ORI(rS, rA, UIMM)   (uint32_t)( POWERPC_OPCODE_ORI | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | UIMM )
#define POWERPC_STD(rS, DS, rA)     (uint32_t)( POWERPC_OPCODE_STD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )
#define POWERPC_LD(rS, DS, rA)      (uint32_t)( POWERPC_OPCODE_LD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )

//
// Branch related fields.
//
#define POWERPC_BRANCH_LINKED    1
#define POWERPC_BRANCH_ABSOLUTE  2
#define POWERPC_BRANCH_TYPE_MASK ( POWERPC_BRANCH_LINKED | POWERPC_BRANCH_ABSOLUTE )

#define POWERPC_BRANCH_CONDITION_MASK ( 0x001F0000 )
#define POWERPC_BRANCH_CONDITION_MASK ( 0x001F0000 )

__attribute__((noinline)) 
static uint64_t PPCGetConditionalBranchAddress(
    uint64_t InstructionAddress,
    uint32_t Instruction
) 
{
    //
    //  BC - Branch Conditional (BEQ BNE BLT BGE)
    //  [Opcode]   [Branch Options]     [Condition Register]         [Address]      [Absolute] [Linked]
    //   0-5            6-10                    11-15                   16-29            30        31
    //  
    //  Example
    //  010000         00100                   00001              0000 0000 0000 01      0         0
    //

    if( ( Instruction & 0xFC000000 ) != POWERPC_OPCODE_BC )
        return 0;

    if ( Instruction & POWERPC_BRANCH_ABSOLUTE ) 
        return Instruction & ~( 0x0000FFFC );

    intptr_t   BranchOffset = Instruction & 0x0000FFFC;

    //
    // If The MSB for branch offset is set make it negative.
    //
    if ( BranchOffset & ( 1 << 15 ) )
        BranchOffset |= ~0x0000FFFF;

    return uint64_t( intptr_t( InstructionAddress + BranchOffset ) );
}

__attribute__((noinline)) 
static uint64_t PPCGetBranchAddress(
    uint64_t InstructionAddress,
    uint32_t Instruction
)
{
    if( ( Instruction & 0xFC000000 ) != POWERPC_OPCODE_B )
        return 0;

    //
    //  B - Branch (B BL BA BLA)
    //  [Opcode]       [Address]              [Absolute] [Linked]
    //    0-5            6-29                     30        31
    //
    //  Example
    //  010010    0000 0000 0000 0000 0000 0001   0         0
    //

    if ( Instruction & POWERPC_BRANCH_ABSOLUTE ) 
        return Instruction & ~( POWERPC_OPCODE_B | POWERPC_BRANCH_TYPE_MASK );

    intptr_t BranchOffset = Instruction & 0x03FFFFFC;

    //
    // If The MSB for branch offset is set make it negative.
    //
    if ( BranchOffset & ( 1 << 25 ) ) 
        BranchOffset |= ~0x03FFFFFF;

    return uint64_t( intptr_t( InstructionAddress + BranchOffset ) );
}

static inline uint64_t PPCGetBranchAddress( 
    uint64_t InstructionAddress
)
{
    return PPCGetBranchAddress( InstructionAddress, *(uint32_t*)InstructionAddress );
}

static inline uint64_t PPCGetConditionalBranchAddress( 
    uint64_t InstructionAddress
)
{
    return PPCGetBranchAddress( InstructionAddress, *(uint32_t*)InstructionAddress );
}

static inline uint64_t PPCGetAnyBranchAddress( uint64_t InstructionAddress )
{
    const auto Address = PPCGetBranchAddress( InstructionAddress );

    if( Address == 0 )
        return PPCGetConditionalBranchAddress( InstructionAddress );

    return Address;
}

#define __mtspr(SPR, Value) \
  __asm__ volatile ("mtspr %0,%1" : : "n" (SPR), "r" (Value))
  
#define __mfspr(SPR) __extension__				\
  ({ unsigned long long __macro_result;					\
  __asm__ volatile ("mfspr %0,%1" : "=r" (__macro_result) : "n" (SPR)); \
  __macro_result; })

#define POWERPC_SPR_SRR0 26
#define POWERPC_SPR_SRR1 27

#define POWERPC_SPR_SPRG0 272
#define POWERPC_SPR_SPRG1 273
#define POWERPC_SPR_SPRG2 274
#define POWERPC_SPR_SPRG3 275

#define __mfsprg0() __mfspr( POWERPC_SPR_SPRG0 )
#define __mfsprg1() __mfspr( POWERPC_SPR_SPRG1 )
#define __mfsprg2() __mfspr( POWERPC_SPR_SPRG2 )
#define __mfsprg3() __mfspr( POWERPC_SPR_SPRG3 )

#define __mtsprg0(Value) __mtspr( POWERPC_SPR_SPRG0, Value )
#define __mtsprg1(Value) __mtspr( POWERPC_SPR_SPRG1, Value )
#define __mtsprg2(Value) __mtspr( POWERPC_SPR_SPRG2, Value )
#define __mtsprg3(Value) __mtspr( POWERPC_SPR_SPRG3, Value )

#define __mfmsrd(L) __extension__				\
  ({ unsigned long long __macro_result;					\
  __asm__ volatile ("mfmsrd %0,%1" : "=r" (__macro_result) : "n" (L)); \
  __macro_result; })

#define __mtmsrd(Value, L) \
  __asm__ volatile ("mtmsrd %0,%1" : : "r" (Value), "n" (L))
  
#define __icbi(Base) \
  __asm__ volatile ("icbi %y0" : "=m" (Base) : : "memory")

static inline void PPCFlushInstructionCache( void* Address, size_t Length )
{
    auto FlushEnd = (char *)Address + Length;

    //
    // Allign address by 128
    //
    auto FlushStart = uint64_t( Address ) & ~127uLL;

    for ( char* CacheLine = (char*)FlushStart; CacheLine < FlushEnd; CacheLine += 128LL )
    {
        
        //
        // Flush data cache.
        //
        __builtin_ppc_dcbst( CacheLine );
        __builtin_ppc_sync( );

        //
        // Flush instruction cache.
        //
        __icbi( CacheLine );
        __builtin_ppc_isync( );
    }
}

static inline void PPCWriteBranchRelative( uint32_t* Instruction, uint64_t BranchDestination, bool Linked )
{
	intptr_t Offset = ( BranchDestination - (uint64_t)Instruction );

	*Instruction = POWERPC_B( Offset, 0, Linked );

	PPCFlushInstructionCache( Instruction, 4 );
}

#endif // !POWER_PC_H