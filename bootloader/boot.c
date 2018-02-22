#include "setup.h"

extern void uart0_init(void);
extern void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);
extern void puts(char *str);
extern void puthex(unsigned int val);

static struct tag *params;

static void setup_start_tag (void)
{
	params = (struct tag *) 0x30000100;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

static void setup_memory_tag(void)
{
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size (tag_mem32);

	params->u.mem.start = 0x30000000;
	params->u.mem.size  = 0x4000000;

	params = tag_next (params);
}

int strlen(char *str)
{
	int i = 0;
	while(str[i])
		i++;
	return i;
}

void strcpy(char *dest, char *src)
{
	while((*dest++ = *src++) != '\0');
}

static void setup_commandline_tag (char *cmdline)
{


	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size = (sizeof (struct tag_header) + strlen (cmdline) + 1 + 3) >> 2;
	strcpy (params->u.cmdline.cmdline, p);
	params = tag_next (params);
}

static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

int main(void)
{
	puts("Start to bootloader:\n\r");
	void (*theKernel)(int zero, int arch, uint params);
	/* 1 initialize UART */
	uart0_init();
	puts("UART intialized: Done\n\r");
	
	/* 2 copy kernel form NAND Flash to SDRAM */
	// uImage = 64 + zImage
	nand_read(0x60000+64, (unsigned char *)0x30008000, 0x200000);
	puts("Copy kernel from Nand: Done\n\r");
	
	/* 3 setup bootargs */
	// these code borrow from uBoot/armlinux/lib_arm
	setup_start_tag();
	setup_memory_tag();
	setup_commandline_tag("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0");
	setup_end_tag();
	puts("Set bootargs: Done\n\r");
	/* 4 bootm */
	puts("Booting......\n\r");
	theKernel = (void (*)(int, int, uint))0x30008000
	// /include/asm-arm/mach-types.h/#define MACH_TYPE_S3C2440 362
	theKernel (0, 362, 0x30000100);

	puts("Bootm: Error\n\r");
	return -1;
}

