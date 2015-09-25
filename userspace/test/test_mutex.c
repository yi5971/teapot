/*
 * mutex.c
 *
 *  Created on: 2015年4月12日
 *      Author: Administrator
 */

#if 0
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
#endif

#if 0

#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//==========================对信号量的封装函数==============================
#define IPC_ID 0x26
 /* 我们必须自己定义 semun 联合类型。 */
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
int WhenError(const char * msg, int eno)
{
    printf("semaphore error[%d]:%s, %s\n", eno, msg, strerror(eno));
    exit(0);
    return -1;
}
/* 获取一个count元信号量的标识符。如果需要则创建这个信号量 */
int CreateSemaphore(int count)
{
//   key_t key;
//   key = ftok("/var/httcsec",IPC_ID);
//   printf("key:%d\n", (int)key);
   int semid;
   semid = semget ((key_t)12345, count, 0666|IPC_CREAT);

   if(EEXIST == semid) printf("exist semid\n");
   return (semid == -1)? WhenError("create sem(semget)", errno):semid;
}
/* 释放信号量。所有用户必须已经结束使用这个信号量。如果失败，返回 -1 */
int FreeSemaphore(int semid)
{
   union semun ignored_argument;
   return semctl(semid, 0, IPC_RMID, ignored_argument); //the second and forth args will be ignored in this case.
}

/* Set a semaphore to a value。*/
int SetSemaphoreValue(int semid, int index, int value)
{
  union semun argument;
  argument.val = value;
  int ret;
  ret= semctl(semid,index,SETVAL,argument);
  return (ret == -1)? WhenError("Set Value(semctl)", errno):ret;
}
int SetSemaphoreValues(int semid, unsigned short * values)
{
  union semun argument;
  argument.array=values;
  int ret;
  ret= semctl(semid,0,SETALL,argument);
  return (ret == -1)? WhenError("Set Values(semctl)", errno):ret;
}
int GetSemaphoreValue(int semid, int index)
{
  int ret = semctl(semid,index,GETVAL,0);
  return (ret == -1)? WhenError("Get Value(semctl)", errno):ret;
}
/* 如sem[index]为负且不超时，则阻塞，超时则返回EAGAIN，否则将信号量减1，返回0*/
int TryLockSemaphore(int semid, int index, int milliseconds)
{
   	int ret;
	struct sembuf operation;
	operation.sem_num = index;
	operation.sem_op = -1;
if(milliseconds<1)
{
	  operation.sem_flg = IPC_NOWAIT;
	  ret= semop (semid, &operation, 1);
}
else
{
	  operation.sem_flg = SEM_UNDO;
      struct timespec timeout;
      timeout.tv_sec = (milliseconds / 1000);
      timeout.tv_nsec = (milliseconds - timeout.tv_sec*1000L)*1000000L;
	  ret= semtimedop(semid, &operation, 1, &timeout);
}
    if(ret == -1)
    {
      if(errno == EAGAIN) return EAGAIN;
    }
	return (ret == -1)? WhenError("Wait(semop)", errno):ret;
}
/* 如果sem[index]为负，则阻塞，直到信号量值为正，然后将其减1*/
int LockSemaphore(int semid, int index)
{
	struct sembuf operation;
	operation.sem_num = index;
	operation.sem_op = -1; /* 减一。 */
	operation.sem_flg = SEM_UNDO; /* 允许撤销操作 */
	int ret;
	ret= semop (semid, &operation, 1);
	return (ret == -1)? WhenError("Wait(semop)", errno):ret;
}
/* 将sem[index]值加一。 这个操作会立即返回，应配合LockSemaphore一起使用。*/
int UnlockSemaphore(int semid, int index)
{
	struct sembuf operation;
	operation.sem_num = index;
	operation.sem_op = 1; /* 加一 */
	operation.sem_flg = SEM_UNDO; /* 允许撤销操作 */
	int ret;
	ret= semop(semid, &operation, 1);
	return (ret == -1)? WhenError("Post(semctl)", errno):ret;
}
//========================================================================
int main()
{
   pid_t pid;
   int semid;
   semid = CreateSemaphore(1);
  SetSemaphoreValue(semid, 0, 1);

   pid = fork();
   if(pid==0)   //child 1
   {
      sleep(1);
      int res;
      res = LockSemaphore(semid, 0);
/* Try LockSemaphore的用法
  res = EAGAIN;
  while(res== EAGAIN)
{
    res= TryLockSemaphore(semid, 0, 1000);   //设置1000ms超时
  //do something
}
*/
      printf("child 1 set[%d]....\n",res);
      // do critical things
      sleep(10);
      UnlockSemaphore(semid, 0);
      printf("child 1 post[%d]....\n",res);
      return 0;
   }

  int dd;
  dd = LockSemaphore (semid, 0);
   printf("parent set 1[%d]....\n",dd);
   sleep(10);
   dd = UnlockSemaphore(semid, 0);
   //sleep(2);
   printf("parent end: ...[%d]\n",dd);
   //FreeSemaphore(semid);
   UnlockSemaphore(semid, 0);
}


#endif


#if 1

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>

#define IPC_ID 0x26

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

static int sem_id = 0;

static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

int main(int argc, char *argv[])
{
	char message = 'X';
	int i = 0;

	key_t key;
	key = ftok("/var/httcsec",IPC_ID);
	//创建信号量
	printf("key:%d\n", (int)key);
	sem_id = semget(key, 1, 0666 | IPC_CREAT);

	if(argc > 1)
	{
		//程序第一次被调用，初始化信号量
		if(!set_semvalue())
		{
			fprintf(stderr, "Failed to initialize semaphore\n");
			exit(EXIT_FAILURE);
		}
		//设置要输出到屏幕中的信息，即其参数的第一个字符
		message = argv[1][0];
		sleep(2);
	}
	for(i = 0; i < 3; ++i)
	{
		//进入临界区
		if(!semaphore_p())
			exit(EXIT_FAILURE);
		//向屏幕中输出数据
		printf("..[%d]..lock\n", getpid());
		//清理缓冲区，然后休眠随机时间
		fflush(stdout);
		sleep(rand() % 5);
		//离开临界区前再一次向屏幕输出数据
		printf("..[%d]..unlock\n", getpid());
		fflush(stdout);
		//离开临界区，休眠随机时间后继续循环
		if(!semaphore_v())
			exit(EXIT_FAILURE);
		printf("..[%d]..waiting\n", getpid());
		fflush(stdout);
	}

	printf("\n%d - finished\n", getpid());

	if(argc > 1)
	{
		//如果程序是第一次被调用，则在退出前删除信号量
		sleep(3);
		del_semvalue();
	}
	exit(EXIT_SUCCESS);
}

static int set_semvalue()
{
	//用于初始化信号量，在使用信号量前必须这样做
	union semun sem_union;

	sem_union.val = 1;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;
	return 1;
}

static void del_semvalue()
{
	//删除信号量
	//union semun sem_union;

	if(semctl(sem_id, 0, IPC_RMID, 0) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p()
{
	//对信号量做减1操作，即等待P（sv）
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;//P()
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		fprintf(stderr, "semaphore_p failed\n");
		return 0;
	}
	return 1;
}

static int semaphore_v()
{
	//这是一个释放操作，它使信号量变为可用，即发送信号V（sv）
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;//V()
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		fprintf(stderr, "semaphore_v failed\n");
		return 0;
	}
	return 1;
}

#endif
