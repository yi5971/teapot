/*
 * mutex.c
 *
 *  Created on: 2015Äê4ÔÂ12ÈÕ
 *      Author: Administrator
 */
#include<sys/file.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>


sem_t * mutex_sem_init(char *mutex_sem)
{
	sem_t *mutex;
	mutex = sem_open(mutex_sem, O_CREAT, SEM_MODE, 0);
	if(mutex == SEM_FAILED)
	{
		printf("sem_open failed! (%s)\n", strerror(errno));
		sem_unlink(MUTEX_SEM_NAME);
		return NULL;
	}
	printf("sem_open success!\n");
	return mutex;
}

int mutex_sem_lock(sem_t *mutex)
{
	sem_wait(mutex);
}

int mutex_sem_unlock(sem_t *mutex)
{
	sem_post(mutex);
}

int mutex_sem_close(sem_t *mutex)
{
	sem_close(mutex);
	sem_unlink(MUTEX_SEM_NAME);
}


int mutex_fcntl_init(char *mutex_filename)
{
	int fd;
	fd = open(mutex_filename, O_WRONLY|O_CREAT, 00777);
	if(-1 == fd)
	{
		printf("open failed!\n");
		return -1;
	}
	return fd;
}

int mutex_fcntl_lock_r(int fd)
{
	struct flock lock;

	lock.l_type = F_RDLCK;
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	return (fcntl(fd,F_SETLKW,&lock));
}

int mutex_fcntl_lock_w(int fd)
{
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	return (fcntl(fd,F_SETLKW,&lock));
}

int mutex_fcntl_unlock(int fd)
{
	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	return (fcntl(fd,F_SETLKW,&lock));
}

void mutex_fcntl_close(int fd)
{
	close(fd);
}


int mutex_flock_init(char *mutex_filename)
{
	int fd;
	fd = open(mutex_filename, O_WRONLY|O_CREAT, 00777);
	if(-1 == fd)
	{
		printf("open failed!\n");
		return -1;
	}
	return fd;
}

int mutex_flock_lock(int fd)
{
	return flock(fd,LOCK_EX);
}

int mutex_flock_unlock(int fd)
{
	return flock(fd,LOCK_UN);
}

void mutex_flock_close(int fd)
{
	close(fd);
}
