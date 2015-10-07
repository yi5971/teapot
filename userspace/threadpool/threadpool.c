#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include "threadpool.h"

#define THREAD_POOL_NUM_MAX 10

#define THREAD_POOL_ON 	0
#define THREAD_POOL_OFF 1

struct thread_pool_worker
{
	void *(*process) (void *arg);//thread deal proc, and process's argument pointer is arg
    void *arg;//arg is pointer of CThread_arg item
    struct thread_pool_worker *next;//thread deal proc list
};

/*�̳߳ؽṹ*/
struct thread_pool
{
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;

    /*����ṹ���̳߳������еȴ�����*/
	struct thread_pool_worker *queue_head;

    /*�Ƿ������̳߳�*/
    int shutdown;
    pthread_t *threadid;

    /*�̳߳��еĻ�߳���Ŀ*/
    int max_thread_num;

    /*��ǰ�ȴ����е�������Ŀ*/
    int cur_queue_size;
};

/* �̳߳ؾ�� */
struct thread_pool_handle
{
	struct thread_pool pool;
};


/*���̳߳��м�������*/
int thread_pool_add_worker(thread_pool_handle *handle, void *(*process) (void *arg), void *arg)
{
	/*����һ��������*/
	struct thread_pool_worker *worker, *newworker;

	newworker = (struct thread_pool_worker *)malloc(sizeof(struct thread_pool_worker));
	if(!newworker)
		return -1;

	newworker->process = process;
	newworker->arg = arg;
	newworker->next = NULL;/*�����ÿ�*/

	pthread_mutex_lock(&(handle->pool.queue_lock));

	/*��������뵽�ȴ�������*/
	worker = handle->pool.queue_head;
	if(worker != NULL)
	{
		while(worker->next != NULL)
			worker = worker->next;
		worker->next = newworker;
	}
	else
	{
		handle->pool.queue_head = newworker;
	}

	handle->pool.cur_queue_size++;
	pthread_mutex_unlock(&(handle->pool.queue_lock));

	/* ���ˣ��ȴ��������������ˣ�����һ���ȴ��̣߳�ע����������̶߳���æµ��
	 * �������߳�������Ӧ���źţ�ֻ�еȵ����߳̽���������ȥ��鹤�����У��ŻῪʼ������*/
	pthread_cond_signal(&(handle->pool.queue_ready));
	return 0;
}

void *thread_routine(void *arg)
{
	struct thread_pool *pool = (struct thread_pool *)arg;
	struct thread_pool_worker *worker;

    printf("starting thread 0x%ld\n", pthread_self());
    while(1)
	{
    	pthread_mutex_lock(&(pool->queue_lock));

        /*����ȴ�����Ϊ0���Ҳ������̳߳أ���������״̬; ע��
        pthread_cond_wait��һ��ԭ�Ӳ������ȴ�ǰ����������Ѻ�����*/
        while(!pool->cur_queue_size && pool->shutdown == THREAD_POOL_ON)
		{
			printf("thread 0x%ld is waiting\n", pthread_self());
			pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
		}

        /*�̳߳�Ҫ������*/
		if(pool->shutdown == THREAD_POOL_OFF)
		{
			/*����break,continue,return����ת��䣬ǧ��Ҫ�����Ƚ���*/
			pthread_mutex_unlock(&(pool->queue_lock));
			printf("thread 0x%ld will exit\n", pthread_self());
			pthread_exit(NULL);
		}

        printf("thread 0x%ld is starting to work\n", pthread_self());
		/*�ȴ����г��ȼ�ȥ1����ȡ�������е�ͷԪ��*/
		pool->cur_queue_size--;
		worker = pool->queue_head;
		pool->queue_head = worker->next;
		pthread_mutex_unlock(&(pool->queue_lock));

		/*���ûص�������ִ������*/
		(*(worker->process))(worker->arg);
		free (worker);
     }
}

thread_pool_handle *thread_pool_init(int num)
{
	thread_pool_handle *handle;
	int i = 0;

	handle = (thread_pool_handle *)malloc(sizeof(thread_pool_handle));
	if(!handle)
	{
		return NULL;
	}

	pthread_mutex_init(&(handle->pool.queue_lock), NULL);
	pthread_cond_init(&(handle->pool.queue_ready), NULL);
	handle->pool.queue_head = NULL;
	handle->pool.max_thread_num = THREAD_POOL_NUM_MAX;
	handle->pool.cur_queue_size = 0;
	handle->pool.shutdown = THREAD_POOL_ON;

	handle->pool.threadid = (pthread_t *)malloc(num * sizeof(pthread_t));
	if(!handle->pool.threadid)
	{
		free(handle);
		return NULL;
	}
	for(i=0;i<num;i++)
	{
		pthread_create(&(handle->pool.threadid[i]), NULL, thread_routine, &handle->pool);
	}

	return handle;
}
void thread_pool_exit(thread_pool_handle *handle)
{
	int i;
	struct thread_pool_worker *worker = NULL;

	if (handle->pool.shutdown == THREAD_POOL_OFF)
		return;/*��ֹ���ε���*/
	handle->pool.shutdown = 1;

	/*�������еȴ��̣߳��̳߳�Ҫ������*/
	pthread_cond_broadcast(&(handle->pool.queue_ready));

	/*�����ȴ��߳��˳�������ͳɽ�ʬ��*/
	for(i=0;i<handle->pool.max_thread_num;i++)
		pthread_join(handle->pool.threadid[i], NULL);
	free(handle->pool.threadid);

	/*���ٵȴ�����*/
	while(handle->pool.queue_head != NULL)
	{
		worker = handle->pool.queue_head;
		handle->pool.queue_head = handle->pool.queue_head->next;
		free(worker);
	}

	/*�������������ͻ�����*/
	pthread_mutex_destroy(&(handle->pool.queue_lock));
	pthread_cond_destroy(&(handle->pool.queue_ready));

	free(handle);

	return;
}

