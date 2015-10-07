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

/*线程池结构*/
struct thread_pool
{
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;

    /*链表结构，线程池中所有等待任务*/
	struct thread_pool_worker *queue_head;

    /*是否销毁线程池*/
    int shutdown;
    pthread_t *threadid;

    /*线程池中的活动线程数目*/
    int max_thread_num;

    /*当前等待队列的任务数目*/
    int cur_queue_size;
};

/* 线程池句柄 */
struct thread_pool_handle
{
	struct thread_pool pool;
};


/*向线程池中加入任务*/
int thread_pool_add_worker(thread_pool_handle *handle, void *(*process) (void *arg), void *arg)
{
	/*构造一个新任务*/
	struct thread_pool_worker *worker, *newworker;

	newworker = (struct thread_pool_worker *)malloc(sizeof(struct thread_pool_worker));
	if(!newworker)
		return -1;

	newworker->process = process;
	newworker->arg = arg;
	newworker->next = NULL;/*别忘置空*/

	pthread_mutex_lock(&(handle->pool.queue_lock));

	/*将任务加入到等待队列中*/
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

	/* 好了，等待队列中有任务了，唤醒一个等待线程；注意如果所有线程都在忙碌，
	 * 不会有线程立刻响应该信号，只有等到有线程结束任务再去检查工作队列，才会开始该任务*/
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

        /*如果等待队列为0并且不销毁线程池，则处于阻塞状态; 注意
        pthread_cond_wait是一个原子操作，等待前会解锁，唤醒后会加锁*/
        while(!pool->cur_queue_size && pool->shutdown == THREAD_POOL_ON)
		{
			printf("thread 0x%ld is waiting\n", pthread_self());
			pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
		}

        /*线程池要销毁了*/
		if(pool->shutdown == THREAD_POOL_OFF)
		{
			/*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/
			pthread_mutex_unlock(&(pool->queue_lock));
			printf("thread 0x%ld will exit\n", pthread_self());
			pthread_exit(NULL);
		}

        printf("thread 0x%ld is starting to work\n", pthread_self());
		/*等待队列长度减去1，并取出链表中的头元素*/
		pool->cur_queue_size--;
		worker = pool->queue_head;
		pool->queue_head = worker->next;
		pthread_mutex_unlock(&(pool->queue_lock));

		/*调用回调函数，执行任务*/
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
		return;/*防止两次调用*/
	handle->pool.shutdown = 1;

	/*唤醒所有等待线程，线程池要销毁了*/
	pthread_cond_broadcast(&(handle->pool.queue_ready));

	/*阻塞等待线程退出，否则就成僵尸了*/
	for(i=0;i<handle->pool.max_thread_num;i++)
		pthread_join(handle->pool.threadid[i], NULL);
	free(handle->pool.threadid);

	/*销毁等待队列*/
	while(handle->pool.queue_head != NULL)
	{
		worker = handle->pool.queue_head;
		handle->pool.queue_head = handle->pool.queue_head->next;
		free(worker);
	}

	/*销毁条件变量和互斥量*/
	pthread_mutex_destroy(&(handle->pool.queue_lock));
	pthread_cond_destroy(&(handle->pool.queue_ready));

	free(handle);

	return;
}

