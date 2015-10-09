/*
 * test_ioctl.c
 *
 *  Created on: 2015Äê4ÔÂ3ÈÕ
 *      Author: Administrator
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include "../channel/channel.h"

int main(int argc, char *argv[])
{
	int rc;
	int type = TEAPOT_CHANNEL_TYPE_SET(TEAPOT_CHANNEL_TYPE_IOCTL, TEAPOT_CHANNEL_DATA_TYPE_CMD);

	rc = teapot_kernel_channel(type, NULL, 2);

	printf("rc = %d\n", rc);

	return 0;
}
