#¹ýÂËÄ³Ð©Ä¿Â¼
#exclude_dirs = utils
#SUBDIRS = $(shell find . -maxdepth 1 -type d ! -name ".*")
#SUBDIRS := $(basename $(patsubst ./%,%,$(SUBDIRS)))
#SUBDIRS := $(filter-out $(exclude_dirs),$(SUBDIRS))

.PHONY:all clean

LIBS += -lpthread -ldl
CFLAGS = -fPIC

allsqlite3 := libsqlite3.so sqlite3
all:$(allsqlite3) build-sqlite3  

libsqlite3.so:sqlite3.o
	$(CC) $^ ${CFLAGS} ${LIBS} -shared -o $@
	
sqlite3:shell.o sqlite3.o 
	$(CC) $^ ${CFLAGS} ${LIBS} -o $@
	
build-sqlite3:
	cp -f sqlite3ext.h $(USERSPACE_INCLUDE_DIR)
	cp -f libsqlite3.so $(USERSPACE_LIBS_DIR)
	cp -f sqlite3 $(USERSPACE_TOOLS_DIR)

clean:
	$(RM) $(allsqlite3) *.o
