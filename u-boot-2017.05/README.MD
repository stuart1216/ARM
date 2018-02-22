# This project is for u-boot.2017.05 transplant on JZ2440

JZ2440 is a very old development board based on Samsung s3c2440 with a ARM920t core. Latest u-boot is going to transplant to this board. 

## Make analysis

### Build relationship

#### .all(Makefile)

```
ALL-y += u-boot.srec u-boot.bin u-boot.sym System.map binary_size_check

```

#### u-boot.bin(Makefile)
```
ifeq ($(CONFIG_OF_SEPARATE),y)
u-boot-dtb.bin: u-boot-nodtb.bin dts/dt.dtb FORCE
	$(call if_changed,cat)

u-boot.bin: u-boot-dtb.bin FORCE
	$(call if_changed,copy)
else
u-boot.bin: u-boot-nodtb.bin FORCE
	$(call if_changed,copy)
endif
```

#### u-boot-nodtb.bin(Makefile)
```
u-boot-nodtb.bin: u-boot FORCE
	$(call if_changed,objcopy)
	$(call DO_STATIC_RELA,$<,$@,$(CONFIG_SYS_TEXT_BASE))
	$(BOARD_SIZE_CHECK)
```

#### u-boot(Makefile)
```
u-boot:	$(u-boot-init) $(u-boot-main) u-boot.lds FORCE
	$(call if_changed,u-boot__)
ifeq ($(CONFIG_KALLSYMS),y)
	$(call cmd,smap)
	$(call cmd,u-boot__) common/system_map.o
endif
```

#### u-boot-init(Makefile)
``` 
u-boot-init := $(head-y)
```

in /arch/arm/Makefile
```
head-y := arch/arm/cpu/$(CPU)/start.o
```

#### u-boot-main(Makefile)
```
u-boot-main := $(libs-y)
```
in /arch/arm/Makefile
```
libs-y += $(machdirs)
```

#### %config
when excute commod: make smdk2440_config
* in Makefile
```c
scripts_basic:
    $(Q)$(MAKE) $(build)=scripts/basic  //build = scripts/Kbuild.include:181：
                                        //build := -f $(srctree)/scripts/Makefile.build obj, 
                                        //@make -f scripts/Makefile.build obj=scripts/basic 
    $(Q)rm -f .tmp_quiet_recordmcount

    %config: scripts_basic outputmakefile FORCE     
    $(Q)$(MAKE) $(build)=scripts/kconfig $@   //@make -f scripts/Makefile.build obj=scripts/kconfig %config 
```

#### if_changed(/scripts/Kbuild.include)
```c
if_changed = $(if $(strip $(any-prereq) $(arg-check)),                       \
	@set -e;                       \
	$(echo-cmd) $(cmd_$(1));       \
	printf '%s\n' 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd);
```

the diagram:
            make
			 ^
			 |
			all
			 ^
			 |
         u-boot.bin
             ^
             |
      u-boot-nodtb.bin
             ^
             |
          u-boot
             ^
             |
u-boot-init  &  u-boot-main
     ^                ^
     |                |
   head.y           libs.y
     ^                ^
     |                |
   start.S          obj-y

## source code analysis

### strat.S

set SVC32 mode

trun off watchdog

mask IRQ

set up frequency divider(but not frequency)

cpu_init_crit

　　clear cache

　　disable mmu

　　lowlevel_init

　　　　set up memory register

_main

　　in arch/arm/lib/cert0.S




## Transplant log

### creating board information
this part of codes borrowed from SMDK5250 in u-Boot-2017.05 and SMDK2410 in u-Boot 2012.04,01
　　arch/arm/Kconfig
　　board/samsung/smdk2440/Kconfig MAINTAINERS Makefile lowlevel_init.S smdk2440.c
　　configs/smdk2440_deconfig
　　include/configs/smdk2440.h
　　
　　
　　

#### /board/samsung/MAINTAINERS
the belowing code needed to be created

　　board/samsung/smdk2440/

　　include/configs/smdk2440.h

　　configs/smdk2440_defconfig

#### /arch/arm/Kconfig

　　add board information

```C
error: asm/arch/s3c24x0_cpu.h: No such file or directory
```
cp /arch/arm/cpu/arm920t/s3c24x0
cp /arch/arm/include/asm/arch-s3c24x0

```C
error: 'MACH_TYPE_SMDK2410' undeclared
```
gd->bd->bi_arch_number = 193; //MACH_TYPE_SMDK2410

```c
error "CONFIG_MTD_UBI_WL_threshold" UNDECLARED
```
/include/configs/smdk2410.h 





