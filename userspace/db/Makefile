#¹ýÂËÄ³Ð©Ä¿Â¼
#exclude_dirs = utils
#SUBDIRS = $(shell find . -maxdepth 1 -type d ! -name ".*")
#SUBDIRS := $(basename $(patsubst ./%,%,$(SUBDIRS)))
#SUBDIRS := $(filter-out $(exclude_dirs),$(SUBDIRS))

.PHONY:all clean

all:
	$(MAKE) -C sqlite3
	@echo "\n\tBuild db end.......\n\n"

clean:
	$(MAKE) -C sqlite3 clean 
