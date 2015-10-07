#过滤某些目录
#exclude_dirs = utils
#SUBDIRS = $(shell find . -maxdepth 1 -type d ! -name ".*")
#SUBDIRS := $(basename $(patsubst ./%,%,$(SUBDIRS)))
#SUBDIRS := $(filter-out $(exclude_dirs),$(SUBDIRS))

PWD := $(shell pwd)

ifndef INCLUDE_DIR
INCLUDE_DIR := $(PWD)/../include
endif

USERSPACE_INCLUDE_DIR := $(PWD)/include
USERSPACE_LIBS_DIR := $(PWD)/libs
USERSPACE_TOOLS_DIR := $(PWD)/tools

export USERSPACE_INCLUDE_DIR
export USERSPACE_LIBS_DIR
export USERSPACE_TOOLS_DIR


#-f后面接配置参数，PIC表示位置无关代码，动态库必须用
CFLAGS = -fPIC -I$(INCLUDE_DIR)/include

ifdef M32

#LDFLAGS用来指定连接库的位置
LDFLAGS +=  -m32 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -L/usr/lib32/i386-linux-gnu

#LDFLAGS用来直接指定连接库
LIBS +=

#CFLAGS用来指定头文件的位置
CFLAGS  +=  -m32 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

endif

libjson_objs = \
	jsonc/arraylist.o \
	jsonc/debug.o \
	jsonc/json_c_version.o \
	jsonc/json_object.o \
	jsonc/json_object_iterator.o \
	jsonc/json_tokener.o \
	jsonc/json_util.o \
	jsonc/linkhash.o \
	jsonc/printbuf.o \
	jsonc/random_seed.o

libthreadpool_objs = threadpool/threadpool.o

SHAREOBJS = \
	channel/ioctl/ioctl.o \
	channel/channel.o \
	$(libjson_objs) \
	$(libthreadpool_objs)


.PHONY:all libteapot-amd64.so libteapot-i386.so test db clean

all:libteapot.so test

libteapot.so: $(SHAREOBJS)
	$(CC) ${LDFLAGS} ${CFLAGS} -shared -o $@ $^
	@echo 
	@echo "Move libteapot.so to /lib ...."
	cp libteapot.so /lib
	ldconfig
	@echo "Move Success!....."

libteapot-i386.so:
	$(RM) $(SHAREOBJS) libteapot.so
	$(MAKE) M32=1  libteapot.so
	mv -f libteapot.so $@
	$(RM) $(SHAREOBJS)

libteapot-amd64.so:
	$(RM) $(SHAREOBJS) libteapot.so
	$(MAKE) libteapot.so
	mv -f libteapot.so $@
	$(RM) $(SHAREOBJS)

# 	-C 表示到指定目录下，再执行make
test:
	$(MAKE) -C test
	
db:
	$(MAKE) -C db

clean:
	$(MAKE) -C test clean
	$(MAKE) -C db clean
	$(RM) libteapot.so $(SHAREOBJS)
	$(RM) $(USERSPACE_INCLUDE_DIR)/* $(USERSPACE_LIBS_DIR)/* $(USERSPACE_TOOLS_DIR)/*
