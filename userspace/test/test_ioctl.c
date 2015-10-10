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
	int type = TEAPOT_CHANNEL_TYPE_SET(TEAPOT_CHANNEL_TYPE_IOCTL, TEAPOT_CHANNEL_DATA_TYPE_CMD_TEST, 0);

	char *data = NULL;
	int len = 0;
	if(argc == 2){
		data = argv[1];
		len = strlen(argv[1])+1;
	}

	rc = teapot_kernel_channel(type, data, len);


	printf("rc = %d\n", rc);

	return 0;
}
