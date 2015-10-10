/*
 * channel.c
 *
 *  Created on: 2015Äê9ÔÂ26ÈÕ
 *      Author: Administrator
 */

#include "channel.h"
#include "./ioctl/ioctl.h"


int teapot_kernel_channel(int type, void *data, int size)
{
	int ret;

	unsigned long r = -1;

	switch(TEAPOT_CHANNEL_TYPE_GET(type))
	{
		case TEAPOT_CHANNEL_TYPE_IOCTL:
			r = teapot_channel_ioctl(TEAPOT_CHANNEL_DATA_TYPE_GET(type), data, size);
			break;

		case TEAPOT_CHANNEL_TYPE_NETLINK:
			//r = teapot_channel_netlink(type, data, size);
			break;

		default:
			return -1;
	}

	return r;
}



