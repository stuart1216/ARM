CC        		= arm-linux-gcc
LD				= arm-linux-ld 
AR				= arm-linux-ar
OBJCOPY			= arm-linux-objcopy 
OBJDUMP			= arm-linux-objdump 

CFLAGS  		:= -Wall -O 
CPPFLAGS		:= -nostdinc -nostdlib -fno-builtin

objs 			:= start.o init.o boot.o 

boot.bin 		:$(objs)
	${LD} 		-Tboot.lds -o boot.elf $^
	${OBJCOPY} 	-O binary -S boot.elf $@
	${OBJDUMP}	-D -M arm boot.elf > boot.distclean

%.o: %.s
	${CC}		$(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.S
	${CC}		$(CPPFLAGS) $(CFLAGS) -C -O $@ $<

clean:
	rm *.o *.dis *.elf *.bin	