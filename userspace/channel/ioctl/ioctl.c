/*
 * ioctl.c
 *
 *  Created on: 2015Äê4ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../channel.h"
#include "ioctl.h"

#define TEAPOT_MISC_DEV "/dev/teapot"

#define TEAPOT_MISC_DEVICE_TYPE  0x7f

int teapot_channel_ioctl(int type, void *data, int size)
{
	int fd, cmd, rc = 0;
	struct teapot_ioctl_data unit;

	fd = open(TEAPOT_MISC_DEV, O_RDWR);
	if(fd == -1){
		printf("fail open devic:%s\n", TEAPOT_MISC_DEV);
		return -1;
	}

	unit.len = size;
	unit.data = data;
	cmd = _IOWR(TEAPOT_MISC_DEVICE_TYPE, type, unit);

	rc = ioctl(fd, cmd, &unit);
	if(rc < 0){
		printf("fail ioctl\n");
		rc = -1;
	}

	close(fd);
	return rc;
}

