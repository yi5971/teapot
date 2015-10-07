/*
 * mutex.c
 *
 *  Created on: 2015Äê4ÔÂ12ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>
#include<sys/file.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/file.h>

#define MUTEX_FILE_NAME	"/var/tmp/flock.teapot"
static int mutex_fd;

int mutex_init(void)
{
	int fd;
	fd = open(MUTEX_FILE_NAME, O_WRONLY|O_CREAT, 00777);
	if(-1 == fd)
	{
		printf("open failed!\n");
		return -1;
	}
	//printf("sem_open success!\n");
	return fd;
}

int mutex_lock(int fd)
{
	return flock(fd,LOCK_EX);
}

int mutex_unlock(int fd)
{
	return flock(fd,LOCK_UN);
}

void mutex_close(int fd)
{
	close(fd);
}


try_lock()
{
	mutex_fd = mutex_init();
	mutex_lock(mutex_fd);
}

try_unlock()
{
	mutex_unlock(mutex_fd);
	mutex_close(mutex_fd);
}

int main(int argc, char *argv[])
{
	int i=1;

	pid_t pid = fork();

	if(pid){
		while(i<10){
			printf("waiting [%d]......\n", getpid());
			try_lock();
//			printf("get mutex, process:%s, pid:%d, i:%d\n", argv[0], getpid(), i);
			printf("lock [%d]......\n", getpid());
			sleep(2);
			i++;
			printf("unlock [%d]......\n", getpid());
			try_unlock();
		}
	}
	else
	{
		while(i<10){
			printf("waiting [%d]......\n", getpid());
			try_lock();
//			printf("get mutex, process:%s, pid:%d, i:%d\n", argv[0], getpid(), i);
			printf("lock [%d]......\n", getpid());
			sleep(2);
			i++;
			printf("unlock [%d]......\n", getpid());
			try_unlock();
		}
	}

	return 0;
}
