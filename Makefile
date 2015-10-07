#
# 
	I386DIR ?= /work/headers/i386
	AMD64DIR = /work/headers/amd64
	SERVER64DIR ?= /work/headers/server64
	ANDROIDDIR ?= /work/headers/android
	
	#KERNELDIR = $(AMD64DIR)/linux-headers-2.6.32-62-generic
	KERNELDIR = /lib/modules/$(shell uname -r)/build
	
	#android
	HX_X86T=$(ANDROIDDIR)/haixin_x68t_4g_kernel_3.4
	BD_W800=$(ANDROIDDIR)/beidou_w800_kernel_3.4
	COS=$(ANDROIDDIR)/cos_kernel_3.4

	PWD := $(shell pwd)/kernel
	INCLUDE_DIR := $(shell pwd)/include
	export INCLUDE_DIR

.PHONY : all android clean amd64 i386 userspace

#HOOK=PATCH ZERO_USER=ON 
all:
	$(MAKE) DEBUG=ON -C $(KERNELDIR) M=$(PWD) modules
#	$(MAKE) DEBUG=ON -C $(KERNELDIR) M=$(PWD)/path_dac modules
	@echo
	$(MAKE) -C userspace
#	$(MAKE) -C client
	
userspace:
	$(MAKE) -C userspace
	
amd64:	
#	构造build编译目录
	-rm -rf build/kernel/amd64
	-mkdir -p build/kernel/amd64
	
#	2.6.32-62 的64版本，去掉。strip用来去掉模块中调试信息
	$(MAKE) -C $(AMD64DIR)/linux-headers-2.6.32-62-generic M=$(PWD) clean
	$(MAKE) ARCH=x86_64 ERR_NULL=ON -C $(AMD64DIR)/linux-headers-2.6.32-62-generic M=$(PWD) modules
	strip --strip-debug src/httcsec.ko
	mv -f src/httcsec.ko build/kernel/amd64/httcsec_2.6.32-62-generic.ko	
	mv -f src/Module.symvers build/kernel/amd64/Module.symvers_2.6.32-62-generic

	touch build/kernel/amd64/time
	
i386:
#	构造build编译目录
	-rm -rf build/kernel/i386
	-mkdir -p build/kernel/i386

#	2.6.32-62 的32版本，含pae,及地址扩展	
	$(MAKE) ARCH=i386 -C $(I386DIR)/linux-headers-2.6.32-62-generic-pae M=$(PWD) clean
	$(MAKE) ARCH=i386  -C $(I386DIR)/linux-headers-2.6.32-62-generic-pae M=$(PWD) modules
	strip --strip-debug src/httcsec.ko
	mv -f src/httcsec.ko build/kernel/i386/httcsec_2.6.32-62-generic-pae.ko
	mv -f src/Module.symvers build/kernel/i386/Module.symvers_2.6.32-62-generic-pae

#	2.6.32-62 的32版本	
	$(MAKE) ARCH=i386 -C $(I386DIR)/linux-headers-2.6.32-62-generic M=$(PWD) clean
	$(MAKE) ARCH=i386  -C $(I386DIR)/linux-headers-2.6.32-62-generic M=$(PWD) modules
	strip --strip-debug src/httcsec.ko
	mv -f src/httcsec.ko build/kernel/i386/httcsec_2.6.32-62-generic.ko
	mv -f src/Module.symvers build/kernel/i386/Module.symvers_2.6.32-62-generic
	
	touch build/kernel/i386/time
	

android: hx_x68t #bd_w800
hx_x68t:
	$(MAKE)  ARCH=arm CROSS_COMPILE=arm-eabi- DEBUG=ON  -C $(HX_X86T) M=$(PWD) modules
bd_w800:
	$(MAKE)  ARCH=arm CROSS_COMPILE=arm-eabi- DEBUG=ON  -C $(BD_W800) M=$(PWD) modules
cos:
	$(MAKE)  ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- DEBUG=ON COS=1 -C $(COS) M=$(PWD) modules
	
clean:
#	$(MAKE) -C client clean
	$(MAKE) -C userspace clean	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
#	$(MAKE) -C $(KERNELDIR) M=$(PWD)/path_dac clean
#	$(MAKE) -C $(HX_X86T) M=$(PWD) clean
#	$(MAKE) -C $(BD_W400) M=$(PWD) clean
