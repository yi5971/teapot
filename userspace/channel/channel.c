/*
 * channel.c
 *
 *  Created on: 2015Äê9ÔÂ26ÈÕ
 *      Author: Administrator
 */

#include "channel.h"
#include "./ioctl/ioctl.h"

static int teapot_channel_ioctl(int type, void *data, int size)
{
	switch(TEAPOT_CHANNEL_DATA_TYPE_GET(type))
	{
		case TEAPOT_CHANNEL_DATA_TYPE_CMD:
			teapot_ioctl(size, 0);
			break;
		case TEAPOT_CHANNEL_DATA_TYPE_READ:
		case TEAPOT_CHANNEL_DATA_TYPE_WRITE:
		case TEAPOT_CHANNEL_DATA_TYPE_RW:
		default:
			return -EPARAM;
	}

	return 0;
}

int teapot_kernel_channel(int type, void *data, int size)
{
	int ret;

	switch(TEAPOT_CHANNEL_TYPE_GET(type))
	{
		case TEAPOT_CHANNEL_TYPE_IOCTL:
			teapot_channel_ioctl(type, data, size);
			break;

		default:
			return -1;
	}

	return 0;
}
