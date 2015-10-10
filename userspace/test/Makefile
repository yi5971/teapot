LIBS += -lteapot -lpthread
CFLAGS += -fPIC -g -I$(USERSPACE_INCLUDE_DIR)
LDFLAGS += -L$(USERSPACE_LIBS_DIR)



test = test_ioctl test_threadpool test_mutex test_port test_proc test_ipc test_blob test_fcntl \
	test_flock test_syslog test_parse_dir test_mnt test_clone test_socket test_route test_tty


all:$(test)
	@echo "\n\tBuild test end.......\n\n"

test_ioctl:test_ioctl.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}

test_threadpool:test_threadpool.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}	

test_mutex:test_mutex.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}

test_fcntl:test_fcntl.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS} -lsqlite3

test_flock:test_flock.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
		
test_proc:test_proc.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
	
test_port:test_port.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
	
test_ipc:test_ipc.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
	
test_blob:test_blob.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS} -lsqlite3
	
test_syslog:test_syslog.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}

test_parse_dir:test_parse_dir.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}

test_mnt:test_mnt.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
	
test_clone:test_clone.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
			
test_socket:test_socket.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}
	
test_route:test_route.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS}

test_tty:test_tty.o
	$(CC) ${CFLAGS} $^ -o $@ ${LDFLAGS} ${LIBS} -lutil
	
clean:
	-$(RM) *.o  $(test)