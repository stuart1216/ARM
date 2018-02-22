/* NAND FLASH regiseter */
#define NFCONF (*((volatile unsigned long *)0x4E000000))
#define NFCONT (*((volatile unsigned long *)0x4E000004))
#define NFCMMD (*((volatile unsigned char *)0x4E000008))
#define NFADDR (*((volatile unsigned char *)0x4E00000C))
#define NFDATA (*((volatile unsigned char *)0x4E000010))
#define NFSTAT (*((volatile unsigned char *)0x4E000020))

/* GPIO */
#define GPHCON              (*(volatile unsigned long *)0x56000070)
#define GPHUP               (*(volatile unsigned long *)0x56000078)

/* UART registers*/
#define ULCON0              (*(volatile unsigned long *)0x50000000)
#define UCON0               (*(volatile unsigned long *)0x50000004)
#define UFCON0              (*(volatile unsigned long *)0x50000008)
#define UMCON0              (*(volatile unsigned long *)0x5000000c)
#define UTRSTAT0            (*(volatile unsigned long *)0x50000010)
#define UTXH0               (*(volatile unsigned char *)0x50000020)
#define URXH0               (*(volatile unsigned char *)0x50000024)
#define UBRDIV0             (*(volatile unsigned long *)0x50000028)

#define TXD0READY   (1<<2)
// for baud rate register: UBRDIV0
#define PCLK            50000000    // PCLK = 50MHz
#define UART_CLK        PCLK         // 
#define UART_BAUD_RATE  115200       // baud rate
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

void uart0_init(void)
{
	GPHCON |= 0xa0;     // TXD0, RXD0 set to pin GPH2 and GPH3
	GPHUP  =  0x0c;     // pull up
	
	// One shop bit and 8 bits per frame; no parity
	// 0000 0011
	ULCON0 = 0x03;
	// using PCLK; Receive and trasmit mode is poling mode
	// 0000 0101
	UCON0 = 0x05;
	// No FIFO
	UFCON0 = 0x00;
	// No flow control
	UMCON0 = 0x00;
	// Baud rate
	UBRDIV0 = UART_BRD;
}

void putc(unsigned char c)
{
	while(!(UTRSTAT0 & (1<<2)));
	UTXH0 = c;
}

void puts(char *str)
{
	int i = 0;
	while (str[i])
	{
		putc(str[i]);
		i++;
	}
}

void puthex(unsigned int val)
{
	int i; 
	int j;
	puts("0x");
	for (i = 0; i < 8; i++) {
		j = ((val >> (7 - i)*4) & 0xf);
		if ((j >= 0) && (j <= 9))
			putc(j + '0');
		else
			putc(j + 'A' - 0xa);
	}
}

void nand_init(void)
{
#define TACLS 0
#define TWRPH0 1
#define TWRPH1 0
	NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
	// initECC, Disable chip select; NAND flash controller disable
	NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
	
}

void nand_select(void)
{
	NFCONT &= ~(1 << 1);
}

void nand_deselect(void)
{
	NFCONT |= (1<<1);
}

void nand_cmd(unsigned char cmd)
{
	volatile int i;
	NFCMMD = cmd;
	for(i = 0; i < 10; i++);
}

void nand_addr(unsigned int addr)
{
	unsigned int page = addr / 2048;
	unsigned int col = addr % 2048;
	volatile int i;

	NFADDR = col & 0xff;
	for(i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for(i = 0; i < 10; i++);

	NFADDR = page & 0xff;
	for(i = 0; i < 10; i++);
	NFADDR = (page >> 8) & 0xff;
	for(i = 0; i < 10; i++);
	NFADDR = (page >> 16) & 0xff;
	for(i = 0; i < 10; i++);
}

void nand_wait_ready(void)
{
	while(!(NFSTAT & 1));
}

unsigned char nand_data(void)
{
	return NFDATA;
}

void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int col = addr % 2048;
	int i = 0;

	nand_select();

	while (i < len) {
		nand_cmd(0x00);
		nand_addr(addr);
		nand_cmd(0x30);
		nand_wait_read();
		for (; (col < 2048) & (i < len); col++){
			buf[i] = nand_data();
			i++;
			addr++;
		}
		col = 0;
	}
	nand_deselect();
}

int isBootFromNorFlash(void)
{
	volatile int *p = (volatile int *)0;
	int val;

	val = *p;
	*p = 0x12345678;
	if(*p == 0x12345678){
		*p = val;
		return 0;
	} else
		return 1;	
}


void copy_code_to_sdram(unsigned char *src, unsigned char *dest, unsigned int len)
{
	int i = 0;
	if (isBootFromNorFlash()){
		while(i < len) {
			dest[i] = src[i];
			i++;
		}
	} else {
		nand_read((unsigned int)src, dest, len);
	}
}

void clear_bss(void)
{
	extern int __bss_start, __bss_end;
	int *p = &__bss_start;
	for(;, p < &__bss_end; p++)
		*p = 0;
}




