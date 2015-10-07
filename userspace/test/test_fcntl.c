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
#include <errno.h>
#include <unistd.h>
#include <sqlite3.h>

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
	printf("sem_open success! fd[%d]\n", fd);
	return fd;
}

int mutex_lock_r(int fd)
{
	struct flock lock;

	lock.l_type = F_RDLCK;
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	return (fcntl(fd,F_SETLKW,&lock));
}

int mutex_lock_w(int fd)
{
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	return (fcntl(fd,F_SETLKW,&lock));
}

int mutex_unlock(int fd)
{
	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	return (fcntl(fd,F_SETLKW,&lock));
}

void mutex_close(int fd)
{
	close(fd);
}


try_lock()
{
	mutex_fd = mutex_init();
	mutex_lock_w(mutex_fd);
}

try_unlock()
{
	mutex_unlock(mutex_fd);
	mutex_close(mutex_fd);
}

int main(int argc, char *argv[])
{
	int i=1;
	char sql_buf[1024] = {0};

	//pid_t pid = fork();

	while(i<5){
		printf("waiting [%d]......\n", getpid());
		try_lock();
//			printf("get mutex, process:%s, pid:%d, i:%d\n", argv[0], getpid(), i);
		printf("lock [%d]......\n", getpid());
		{
			int rc;
			sqlite3 *db = NULL;
			char *errmsg;

			rc = sqlite3_open("./test.db", &db);
			if(rc)
			{
				sqlite3_close(db);
				printf("Can't open database: %s\n", sqlite3_errmsg(db));
				try_unlock();
				return NULL;
			}

			rc = sqlite3_exec(db, "CREATE TABLE if not exists test_table(id INTEGER PRIMARY KEY, flag INTEGER, desc TEXT);", NULL,NULL,&errmsg);
			if (rc != SQLITE_OK)
			{
				sqlite3_free(errmsg);
				sqlite3_close(db);
				printf("Can't create table: %s\n", sqlite3_errmsg(db));
				try_unlock();
				return NULL;
			}

			snprintf(sql_buf, 1024, "INSERT INTO test_table VALUES(NULL, %d, '%s');", i, "test");
			rc = sqlite3_exec(db, sql_buf, NULL, NULL, &errmsg);
			if(rc != SQLITE_OK)
			{
				printf("Can't insert database: %s\n", errmsg);
				sqlite3_free(errmsg);
				sqlite3_close(db);
				try_unlock();
				return -1;
			}

			sqlite3_close(db);
		}
		sleep(5);
		i++;
		printf("unlock [%d]......\n", getpid());
		try_unlock();
		sleep(5);
	}

	return 0;
}
