#ifndef __USERSPACE_THREAD_POOL__
#define __USERSPACE_THREAD_POOL__

/* 线程池句柄 */
typedef struct thread_pool_handle thread_pool_handle;

/*向线程池中加入任务*/
int thread_pool_add_worker(thread_pool_handle *handle, void *(*process) (void *arg), void *arg);

thread_pool_handle *thread_pool_init(int num);
void thread_pool_exit(thread_pool_handle *handle);

#endif /*__USERSPACE_THREAD_POOL__*/


/*
 * 线程池需要改进的地方：
 * 1，当任务过多的时候，当前提供的固定线程个数，不能及时满足处理任务。需要线程池的线程在忙时，能自动增长；闲时，能自动销毁，保留少部分空闲等待。
 * 2，任务队列最好有优先级处理方案，等级or加权处理，现在只是FIFO
 */
