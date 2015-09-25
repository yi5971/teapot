#过滤某些目录
#exclude_dirs = utils
#SUBDIRS = $(shell find . -maxdepth 1 -type d ! -name ".*")
#SUBDIRS := $(basename $(patsubst ./%,%,$(SUBDIRS)))
#SUBDIRS := $(filter-out $(exclude_dirs),$(SUBDIRS))

.PHONY:all clean

all:
	$(MAKE) -C sqlite3
	@echo "\n\tBuild db end.......\n\n"
# 	-C 表示到指定目录下，再执行make

clean:
	$(MAKE) -C sqlite3 clean 
