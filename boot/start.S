.globl start


#define Mode_USR 0x10
#define Mode_FIQ 0x11
#define Mode_IRQ 0x12
#define Mode_SVC 0x13
#define Mode_ABT 0x17
#define Mode_UND 0x1B
#define Mode_SYS 0x1F

#define I_Bit    0x80 
#define F_Bit    0x40 
		
		
start:	
.rept	7
    nop
.endr
    mov r0, r0
    b reset
.word	0x016f2818 // _magic_sig
.word	_magic_start
.word	_magic_end
.word	0x04030201

reset:
    //cpsid if // mask irq & fiq

/* Check for HYP mode */
    mrs r0, CPSR
    and r0, r0, #0x1F
    mov r8, #0x1A
    cmp r0, r8
    beq overHyped

continueBoot: /* Continue with bootup */
    b no_hyp
    b . /* spin on return from main */

overHyped: /* Get out of HYP mode */
// TODO: HYP stub to allow HVC call
    adr r1, continueBoot
    msr ELR_hyp, r1
    mrs r1, CPSR
    and r1, r1, #0x1f
    orr r1, r1, #0x13
    msr SPSR_hyp, r1
    eret
no_hyp:

    // Enable IRQ FIQ (Clear PRIMASK)
    cpsie if

    //  CPACR (Coprocessor Access Control Register)
    mrc     p15, 0, r0, c1, c0, 2
    orr     r0, r0, #(0x3  <<20) // cp10[21:20] b11~Full access (all PL)
    orr     r0, r0, #(0x3  <<22) // cp11[22:23] b11~Full access (all PL)
    mcr     p15, 0, r0, c1, c0, 2
    isb

    // FPEXC (Enable Advanced SIMD and VFP)
    mov     r0, #(0x1 << 30)
	mcr     p10, 7, r0, cr8, cr0, 0
 
    ldr sp, =svc_stack_end
	bl  page_table_init

    // VBAR (Install Exception Vectors)
	ldr r0, =__exc_vectors
	mcr p15, 0, r0, c12, c0, 0

	msr     CPSR_c, #Mode_IRQ | I_Bit | F_Bit
	ldr	sp, =irq_stack_end

	msr     CPSR_c, #Mode_ABT | I_Bit | F_Bit
	ldr	sp, =abt_stack_end

    msr     CPSR_c, #Mode_SVC | I_Bit | F_Bit
	ldr	pc, =main	@ jump to OS entry

.align 12
.globl __exc_vectors
__exc_vectors:
	    B	reset
        LDR     PC, Undefined_Addr
        LDR     PC, SVC_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP     @ Reserved for secure monitor calls
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr
.align 2
Undefined_Addr:	 .word     OS_CPU_UndefHandler //OS_CPU_ARM_ExceptUndefInstrHndlr
SVC_Addr:        .word     OS_CPU_ARM_ExceptSwiHndlr
Prefetch_Addr:   .word     OS_CPU_AbortHandler //OS_CPU_ARM_ExceptPrefetchAbortHndlr
Abort_Addr:      .word     OS_CPU_AbortHandler //OS_CPU_ARM_ExceptDataAbortHndlr
IRQ_Addr:        .word     OS_CPU_ARM_ExceptIrqHndlr
FIQ_Addr:        .word     OS_CPU_ARM_ExceptFiqHndlr


	.data 
	.align 5

	.space 16384
irq_stack_end:
	.space 16384
abt_stack_end:
	.space 16384
svc_stack_end:
