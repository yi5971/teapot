/*
 * channel.c
 *
 *  Created on: 2015Äê4ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include "channel.h"
#include "miscdev/miscdev.h"

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
