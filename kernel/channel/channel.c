/*
 * channel.c
 *
 *  Created on: 2015Äê4ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include <linux/mutex.h>
#include <channel.h>
#include "miscdev/miscdev.h"
#include "channel.h"


#define CHANNEL_ARRAY_MAX 256
static channel_func channel_array[TEAPOT_CHANNEL_TYPE_MAX][CHANNEL_ARRAY_MAX] = {0};

static DEFINE_MUTEX(channel_lock);

int teapot_channel_transaction(int nr, void *data, int size)
{
	unsigned long r = -ENOTTY;
	int type = TEAPOT_CHANNEL_TYPE_GET(nr), index = TEAPOT_CHANNEL_DATA_TYPE_GET(nr);
	mutex_lock(&channel_lock);

	if(type >= 0 && type < TEAPOT_CHANNEL_TYPE_MAX
			&& index >= 0 && index < CHANNEL_ARRAY_MAX
			&& channel_array[type][index])
		r = channel_array[type][index](index, data, size);

	mutex_unlock(&channel_lock);
	return r;
}


int teapot_channel_register(int type, int index, channel_func func)
{
	int r;
	mutex_lock(&channel_lock);

	if(type < 0 || type >= TEAPOT_CHANNEL_TYPE_MAX
			|| index < 0 || index >= CHANNEL_ARRAY_MAX
			|| channel_array[type][index])
		r = -EPERM;
	else
		channel_array[type][index] = func;

	mutex_unlock(&channel_lock);
	return r;
}


int teapot_channel_unregister(int type, int index)
{
	if(type >=0 && type < TEAPOT_CHANNEL_TYPE_MAX
			&& index >=0 && index < CHANNEL_ARRAY_MAX){
		mutex_lock(&channel_lock);
		channel_array[type][index] = 0;
		mutex_unlock(&channel_lock);
	}

	return 0;
}


int channel_init(void)
{
	int r = 0;
	if((r = miscdev_init())){
		return r;
	}
//	if((r = netlink_init())){
//		goto out_miscdev;
//	}
//	goto out;
//out_miscdev:
//    miscdev_exit();
//out:
	return r;
}


void channel_exit(void)
{
	miscdev_exit();
//	netlink_exit();
}
