/*
 * mutex.c
 *
 *  Created on: 2015��4��12��
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
//==========================���ź����ķ�װ����==============================
#define IPC_ID 0x26
 /* ���Ǳ����Լ����� semun �������͡� */
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
/* ��ȡһ��countԪ�ź����ı�ʶ���������Ҫ�򴴽�����ź��� */
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
/* �ͷ��ź����������û������Ѿ�����ʹ������ź��������ʧ�ܣ����� -1 */
int FreeSemaphore(int semid)
{
   union semun ignored_argument;
   return semctl(semid, 0, IPC_RMID, ignored_argument); //the second and forth args will be ignored in this case.
}

/* Set a semaphore to a value��*/
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
/* ��sem[index]Ϊ���Ҳ���ʱ������������ʱ�򷵻�EAGAIN�������ź�����1������0*/
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
/* ���sem[index]Ϊ������������ֱ���ź���ֵΪ����Ȼ�����1*/
int LockSemaphore(int semid, int index)
{
	struct sembuf operation;
	operation.sem_num = index;
	operation.sem_op = -1; /* ��һ�� */
	operation.sem_flg = SEM_UNDO; /* ���������� */
	int ret;
	ret= semop (semid, &operation, 1);
	return (ret == -1)? WhenError("Wait(semop)", errno):ret;
}
/* ��sem[index]ֵ��һ�� ����������������أ�Ӧ���LockSemaphoreһ��ʹ�á�*/
int UnlockSemaphore(int semid, int index)
{
	struct sembuf operation;
	operation.sem_num = index;
	operation.sem_op = 1; /* ��һ */
	operation.sem_flg = SEM_UNDO; /* ���������� */
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
/* Try LockSemaphore���÷�
  res = EAGAIN;
  while(res== EAGAIN)
{
    res= TryLockSemaphore(semid, 0, 1000);   //����1000ms��ʱ
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
	//�����ź���
	printf("key:%d\n", (int)key);
	sem_id = semget(key, 1, 0666 | IPC_CREAT);

	if(argc > 1)
	{
		//�����һ�α����ã���ʼ���ź���
		if(!set_semvalue())
		{
			fprintf(stderr, "Failed to initialize semaphore\n");
			exit(EXIT_FAILURE);
		}
		//����Ҫ�������Ļ�е���Ϣ����������ĵ�һ���ַ�
		message = argv[1][0];
		sleep(2);
	}
	for(i = 0; i < 3; ++i)
	{
		//�����ٽ���
		if(!semaphore_p())
			exit(EXIT_FAILURE);
		//����Ļ���������
		printf("..[%d]..lock\n", getpid());
		//����������Ȼ���������ʱ��
		fflush(stdout);
		sleep(rand() % 5);
		//�뿪�ٽ���ǰ��һ������Ļ�������
		printf("..[%d]..unlock\n", getpid());
		fflush(stdout);
		//�뿪�ٽ������������ʱ������ѭ��
		if(!semaphore_v())
			exit(EXIT_FAILURE);
		printf("..[%d]..waiting\n", getpid());
		fflush(stdout);
	}

	printf("\n%d - finished\n", getpid());

	if(argc > 1)
	{
		//��������ǵ�һ�α����ã������˳�ǰɾ���ź���
		sleep(3);
		del_semvalue();
	}
	exit(EXIT_SUCCESS);
}

static int set_semvalue()
{
	//���ڳ�ʼ���ź�������ʹ���ź���ǰ����������
	union semun sem_union;

	sem_union.val = 1;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;
	return 1;
}

static void del_semvalue()
{
	//ɾ���ź���
	//union semun sem_union;

	if(semctl(sem_id, 0, IPC_RMID, 0) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p()
{
	//���ź�������1���������ȴ�P��sv��
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
	//����һ���ͷŲ�������ʹ�ź�����Ϊ���ã��������ź�V��sv��
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
