
.text
	.section .start,"ax",@progbits
	.align 3
	.global _start
	.type   _start, @function
	.extern PayloadEntry

#include "Config.h"

#ifdef PS3_HEN

//
// Return gadget in iotcl handler.
//
.set IOCTL_RETURN_ADDR, 0x8000000000129E28

//
// Entrypoint for HEN.
//
_start:
.set STACK_SIZE, 0x80

	mfsprg0 %r9
	ld      %r3, 0x68(%r9)
	ld      %r3, 0(%r3)
	ld      %r1, 0x50(%r3)
	addi    %r1, %r1, 0xD40
	
	mflr	%r0
	std 	%r0, 0x10(%r1)
	std 	%r2, 0x28(%r1)
	stdu 	%r1, -STACK_SIZE(%r1)

	mr      %r3, %r2

	lis     %r2, __toc@highest
	sldi    %r2, %r2, 32
	oris    %r2, %r2, __toc@h
	ori     %r2, %r2, __toc@l

	mr      %r4, %r2

	bl 	    PayloadEntry

	// cmpdi   cr7, %r3, 0
	// bne     cr7, 2f
	
	addi 	%r1, %r1, STACK_SIZE
	ld	    %r2, 0x28(%r1)
	ld      %r0, 0x10(%r1)
	ld      %r3, -0x1d60(%r2)
	li      %r5,0
	li      %r6,0
	
	lis     %r0, IOCTL_RETURN_ADDR@highest
	ori     %r0, %r0, IOCTL_RETURN_ADDR@higher
	sldi    %r0, %r0, 32
	oris    %r0, %r0, IOCTL_RETURN_ADDR@h
	ori     %r0, %r0, IOCTL_RETURN_ADDR@l
	mtctr   %r0
	bctr

#else

//
// Entrypoint for debugging.
//
_start:
.set STACK_SIZE, 0x220
	//
	// Setup stack.
	//
	mflr  %r0
	std   %r0,  0x10(%r1)
	std   %r2,  0x28(%r1)
	stdu  %r1,  -STACK_SIZE(%r1)

	mr    %r3, %r2

	//
    // Load the TOC of our payload.
    //
	lis   %r2, __toc@highest
	sldi  %r2, %r2, 32
	oris  %r2, %r2, __toc@h
	ori   %r2, %r2, __toc@l
	
	//
	// PayloadEntry( KernelRTOC, PayloadRTOC )
	//
	mr    %r4, %r2
	bl    .PayloadEntry

	//
	// Cleanup stack and return.
	//
	addi  %r1, %r1, STACK_SIZE
	ld    %r2, 0x28(%r1)
	ld    %r0, 0x10(%r1)
	mtlr  %r0
	blr

#endif