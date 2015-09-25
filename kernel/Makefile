ifdef DEBUG
EXTRA_CFLAGS += -DDEBUG
endif
#ifdef COS
#EXTRA_CFLAGS += -D__COS__
#endif

ARCH_$(ARCH)=1
obj-m += teapot.o

ifeq ($(ARCH), $(filter $(ARCH),x86_64 i386))

teapot-objs :=	main.o \
				channel/channel.o channel/miscdev/miscdev.o

endif