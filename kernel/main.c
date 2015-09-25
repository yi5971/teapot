/*
 * main.c
 *
 *  Created on: 2015Äê4ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include <linux/module.h>
#include <linux/kthread.h>
#include "channel/channel.h"

#define ACC_MODE1(x) ("\000\004\002\006"[(x)&00000003])

static struct task_struct *tsk;

static int thread_func(void *data)
{
	int time_count = 0;

	do{
		if(time_count<3) printk("ACC_MODE[%d]\n", ACC_MODE1(time_count));
		printk("Thread run times[%d]\n", ++time_count);
		schedule_timeout_interruptible(3 * HZ);
	}while(!kthread_should_stop());

	return time_count;
}

static int __init teapot_init(void)
{
	struct module *kmodule = THIS_MODULE;

	printk("Module Name:%s\n", kmodule->name);
	printk("Module version:%s\n", kmodule->version);

	channel_init();

	tsk = kthread_run(thread_func, NULL, "mythread%d", 1);

	return 0;
}


static void teapot_exit(void)
{
	kthread_stop(tsk);
	channel_exit();
	printk("exit\n");
	return;
}



MODULE_AUTHOR("Kong");
MODULE_DESCRIPTION("TEAPOT");
MODULE_VERSION("0.0.1");
MODULE_LICENSE("GPL");

module_init(teapot_init);
module_exit(teapot_exit);

