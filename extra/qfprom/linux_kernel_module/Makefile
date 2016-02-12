KDIR 	:= /path/to/kernel/
PWD 	:= $(shell pwd)
CCPATH 	:= /path/to/arm-linux-androideabi

VERSION = 3
PATCHLEVEL = 0
SUBLEVEL = 8
EXTRAVERSION =

obj-m := tz_qfprom.o
tz_qfprom-objs := server.o qfprom.o main.o

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CCPATH)/arm-linux-androideabi- -C $(KDIR) M=$(PWD) modules 
	$(CCPATH)/arm-linux-androideabi-strip --strip-unneeded tz_qfprom.ko -o $(PWD)/build/tz_qfprom.ko
clean:
	rm -f *.o *.mod.c Module.symvers Module.markers modules.order \.*.o.cmd \.*.ko.cmd \.*.o.d *.ko
	rm -rf \.tmp_versions	
	rm -rf build/* 