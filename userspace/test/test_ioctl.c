/*
 * test_ioctl.c
 *
 *  Created on: 2015Äê4ÔÂ3ÈÕ
 *      Author: Administrator
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include "../channel/ioctl/ioctl.h"
#include "../../include/channel.h"

#define TEAPOT_MISC_DEVICE_TYPE  0x7f
#define TEAPOT_IOCTL_TEST _IO(TEAPOT_MISC_DEVICE_TYPE, 2)

int main(int argc, char *argv[])
{
	int rc;

	rc = teapot_ioctl(TEAPOT_IOCTL_TEST, NULL);

	printf("rc = %d\n", rc);
	printf("TEAPOT_CHANNEL_IOCTL_KERNEL_READ:%d\n", TEAPOT_CHANNEL_IOCTL_KERNEL_READ);

	return 0;
}
