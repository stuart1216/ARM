# This project is for u-boot.2017.05 transplant on JZ2440

JZ2440 is a very old development board based on Samsung s3c2440 with a ARM920t core. Latest u-boot is going to transplant to this board. 


## Log

### 1. Creat board information

\arch\arm\cpu\arm920t\cpu.c

\arch\arm\cpu\arm920t\interrupts.c

\arch\arm\cpu\arm920t\Makefile

\arch\arm\cpu\arm920t\start.S

\arch\arm\Kconfig

\board\samsung\smdk2440\Kconfig

\board\samsung\smdk2440\lowlevel_init.S

\board\samsung\smdk2440\MAINTAINERS

\board\samsung\smdk2440\Makefile

\board\samsung\smdk2440\smdk2440.c

\configs\smdk2410_deconfig

\include\configs\smdk2440.h

#### debug

----
Q:make
```C
lib/asm-offsets.c:1: error: bad value (armv4t) for -march= switch
lib/asm-offsets.c:1: error: bad value (armv4t) for -mtune= switch
```
A: make ARCH=arm CROSS_COMPILE=arm-linux- 

### 2. setup serial port 

