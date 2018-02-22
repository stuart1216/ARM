/*
 * bootloader to JZ2440
 *
 * watchdog
 * clock
 * SDRAM
 * nand_init
 * copy_code_to_sdram
 */
#define MEM_CTL_BASE    0x48000000

.text
.global _start 
_start:

// 1 watchdog turn off
	ldr r0, =0x53000000
	mov r1, #0
	str r1, [r0]

// 2 setup clock
	ldr r0, =0x4c000014  
	mov r1, #0x05			// FCLK:HCLK:PCLK = 1:2:4, HDIVN = 1, PDIVN = 1
	str r1, [r0]
	// set asynchronous bus mode
	mcr p15, 0, r1, c1, c0, 0
	orr r1, r1, #0xc0000000
	mcr p15, 0, r1, c1, c0, 0
	// set FCLK = 400MHz
	ldr r0, =0x4c000004
	ldr r1, ((0x5c << 12)|(0x01<<4)|(0x01<<0))
	str r1, [r0]
	// turn on ICAHE
	mcr p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<12)
	mcr p15, 0, r0, c1, c0, 0

// 3 SDRAM initialize
	ldr r0, MEM_CTL_BASE
	addr r1, sdram_config 
	add r3, #(13*4)
1:
	ldr r2, [r1], #4
	str r2, [r0], #4
	cmp r0, r3
	bne 1b

// relocate program from NAND TO SDRAM
	ldr sp, #0x34000000      // the base addr of SDRAM is 0x30000000, the size is 64M
	bl  nand_init

	mov r0, #0
	ldr r1, =_start
	ldr r2, =_bss_start
	sub r2, r2, r1

	bl copy_code_to_sdram
	bl clear_bss

// load main
	ldr lr, =halt 
	ldr pc, main 
halt:
	b halt

sdram_config:
	.long 0x22011110	 //BWSCON
	.long 0x00000700	 //BANKCON0
	.long 0x00000700	 //BANKCON1
	.long 0x00000700	 //BANKCON2
	.long 0x00000700	 //BANKCON3  
	.long 0x00000700	 //BANKCON4
	.long 0x00000700	 //BANKCON5
	.long 0x00018005	 //BANKCON6
	.long 0x00018005	 //BANKCON7
	.long 0x008C04F4	 // REFRESH
	.long 0x000000B1	 //BANKSIZE
	.long 0x00000030	 //MRSRB6
	.long 0x00000030	 //MRSRB7