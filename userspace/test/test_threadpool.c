/*
 * test_threadpool.c
 *
 *  Created on: 2015Äê4ÔÂ8ÈÕ
 *      Author: Administrator
 */

#include <stdio.h>
#include "../threadpool/threadpool.h"

void *try_printf(int *num)
{
	static int ii = 0;
	sleep(10);
	printf("pthreadid:[0x%x], num:%d, ii:%d\n", pthread_self(), *num, ii);
	return NULL;
}

int main(int argc, char *argv[])
{
	thread_pool_handle *handle;
	int i;

	handle = thread_pool_init(10);
	if(!handle){
		printf("thread_pool_init error!\n");
		return -1;
	}

	sleep(3);
	for(i=0;i<10;i++){
		thread_pool_add_worker(handle, try_printf, &i);
		sleep(1);
	}

	sleep(10);
	thread_pool_exit(handle);
}
