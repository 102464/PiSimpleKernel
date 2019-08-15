// AArch32 mode



.section .text

.global __vector_reset

.equ DISABLE_IRQ, 0x80
.equ DISABLE_FIQ, 0x40
.equ SYS_MOD,	  0x1f
.equ IRQ_MOD,     0x12
.equ FIQ_MOD,     0x11
.equ SVC_MOD,     0x13
.equ ABT_MOD,     0x17
.equ UND_MOD,     0x1B

.equ MEM_SIZE,   0x10000000
.equ TEXT_BASE,  0x8000
.equ STACK_SIZE, 0x100000
.equ _SVC_STACK, (TEXT_BASE + MEM_SIZE - 4)
.equ _IRQ_STACK, (_SVC_STACK - STACK_SIZE)
.equ _FIQ_STACK, (_IRQ_STACK - STACK_SIZE)
.equ _ABT_STACK, (_FIQ_STACK - STACK_SIZE)
.equ _UND_STACK, (_ABT_STACK - STACK_SIZE)
.equ _SYS_STACK, (_UND_STACK - STACK_SIZE)

// Entry point for the kernel.
// r15 -> should begin execution at 0x8000.
// r0 -> 0x00000000
// r1 -> 0x00000C42
// r2 -> 0x00000100 - start of ATAGS

__vector_reset:
	// Stop extra cores
	mrc p15, 0, r5, c0, c0, 5
	and r5, r5, #3
	cmp r5, #0
	bne halt
	// Set up stack
	msr cpsr_c, #(DISABLE_IRQ|DISABLE_FIQ|SVC_MOD)
	ldr sp, =_SVC_STACK

	msr cpsr_c, #(DISABLE_IRQ|DISABLE_FIQ|IRQ_MOD)
	ldr sp, =_IRQ_STACK

	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|FIQ_MOD)
	ldr sp, =_FIQ_STACK

	msr cpsr_c, #(DISABLE_IRQ|DISABLE_FIQ|ABT_MOD)
	ldr sp, =_ABT_STACK

	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|UND_MOD)
	ldr sp, =_UND_STACK

	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|SYS_MOD)
	ldr sp, =_SYS_STACK

	// Clear BSS
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b check
clear:
	stmia r4!, {r5-r8}
check:
	cmp r4, r9
	blo clear

	ldr r3, =kernel_main
	blx r3

halt:
	wfe
	b halt