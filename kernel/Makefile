ifdef DEBUG
EXTRA_CFLAGS += -DDEBUG
endif
#ifdef COS
#EXTRA_CFLAGS += -D__COS__
#endif

#ifeq ($(ARCH), $(filter $(ARCH),x86_64 i386))

ARCH_$(ARCH)=1
obj-m += teapot.o
teapot-objs :=	main.o \
				channel/channel.o channel/miscdev/miscdev.o

#endif