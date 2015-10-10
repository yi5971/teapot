/*
 * channel.h
 *
 *  Created on: 2015Äê4ÔÂ2ÈÕ
 *      Author: Administrator
 */

#ifndef TEAPOTSET_KERNEL_CHANNEL_CHANNEL_H_
#define TEAPOTSET_KERNEL_CHANNEL_CHANNEL_H_

typedef int (*channel_func)(int data_type, void *data, int size);

int teapot_channel_transaction(int type, void *data, int size);

int teapot_channel_register(int type, int index, channel_func func);
int teapot_channel_unregister(int type, int index);

int channel_init(void);
void channel_exit(void);



#endif /* TEAPOTSET_KERNEL_CHANNEL_CHANNEL_H_ */
