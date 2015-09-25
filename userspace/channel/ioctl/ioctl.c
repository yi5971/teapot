/*
 * ioctl.c
 *
 *  Created on: 2015Äê4ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ioctl.h"

#define TEAPOT_MISC_DEV "/dev/teapot"


int teapot_ioctl(unsigned long cmd,unsigned long param)
{
	int fd, rc = 0;

	fd = open(TEAPOT_MISC_DEV, O_RDWR);
	if(fd == -1){
		printf("fail open devic:%s\n", TEAPOT_MISC_DEV);
		return -1;
	}

	rc = ioctl(fd, cmd, param);
	if(rc < 0){
		printf("fail ioctl\n");
		rc = -1;
	}

	close(fd);
	return rc;
}

