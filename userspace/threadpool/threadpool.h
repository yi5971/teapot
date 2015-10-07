#ifndef __USERSPACE_THREAD_POOL__
#define __USERSPACE_THREAD_POOL__

/* �̳߳ؾ�� */
typedef struct thread_pool_handle thread_pool_handle;

/*���̳߳��м�������*/
int thread_pool_add_worker(thread_pool_handle *handle, void *(*process) (void *arg), void *arg);

thread_pool_handle *thread_pool_init(int num);
void thread_pool_exit(thread_pool_handle *handle);

#endif /*__USERSPACE_THREAD_POOL__*/


/*
 * �̳߳���Ҫ�Ľ��ĵط���
 * 1������������ʱ�򣬵�ǰ�ṩ�Ĺ̶��̸߳��������ܼ�ʱ���㴦��������Ҫ�̳߳ص��߳���æʱ�����Զ���������ʱ�����Զ����٣������ٲ��ֿ��еȴ���
 * 2�����������������ȼ����������ȼ�or��Ȩ��������ֻ��FIFO
 */
