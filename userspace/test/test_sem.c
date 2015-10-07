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
#include<fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>

#define SEM_MODE	(S_IRUSR|S_IWUSR)
#define MUTEX_SEM_NAME	"sem.teapot"

sem_t * mutex_init(void)
{
	sem_t *mutex;
	mutex = sem_open(MUTEX_SEM_NAME, O_CREAT, SEM_MODE, 1);
	if(mutex == SEM_FAILED)
	{
		printf("sem_open failed!\n");
		sem_unlink(MUTEX_SEM_NAME);
		return NULL;
	}
	//printf("sem_open success!\n");
	return mutex;
}

int mutex_lock(sem_t *mutex)
{
	sem_wait(mutex);
}

int mutex_unlock(sem_t *mutex)
{
	sem_post(mutex);
}

int mutex_close(sem_t *mutex)
{
	sem_close(mutex);
	sem_unlink(MUTEX_SEM_NAME);
}


try_lock(sem_t **mutex)
{
	*mutex = mutex_init();
	mutex_lock(*mutex);
}

try_unlock(sem_t *mutex)
{
	mutex_unlock(mutex);
	mutex_close(mutex);
}

int main(int argc, char *argv[])
{
	sem_t *mutex;
	int i=1;

	pid_t pid = fork();

	if(pid){
		while(i<10){
			printf("waiting [%d]......\n", getpid());
			try_lock(&mutex);
//			printf("get mutex, process:%s, pid:%d, i:%d\n", argv[0], getpid(), i);
			printf("lock [%d]......\n", getpid());
			sleep(1);
			i++;
			printf("unlock [%d]......\n", getpid());
			try_unlock(mutex);
		}
	}
	else
	{
		while(i<10){
			printf("waiting [%d]......\n", getpid());
			try_lock(&mutex);
//			printf("get mutex, process:%s, pid:%d, i:%d\n", argv[0], getpid(), i);
			printf("lock [%d]......\n", getpid());
			sleep(1);
			i++;
			printf("unlock [%d]......\n", getpid());
			try_unlock(mutex);
		}
	}

	return 0;
}
