/*
 * channel.h
 *
 *  Created on: 2015��4��3��
 *      Author: Administrator
 */

#ifndef TEAPOTSET_INCLUDE_CHANNEL_H_
#define TEAPOTSET_INCLUDE_CHANNEL_H_

// error code
#define EPARAM	-1;


// 32bits, split 16:8:8, reserve:data_type:type
#define TEAPOT_CHANNEL_TYPE_BITS	8
#define TEAPOT_CHANNEL_DATA_TYPE_BITS	8
#define TEAPOT_CHANNEL_RESERVE_BITS	16


// channel type, 8bits
enum{
	TEAPOT_CHANNEL_TYPE_IOCTL = 0X01,
	TEAPOT_CHANNEL_TYPE_NETLINK,
	TEAPOT_CHANNEL_TYPE_MAX
};

#define TEAPOT_CHANNEL_TYPE_MASK	((1<<TEAPOT_CHANNEL_TYPE_BITS)-1)
#define TEAPOT_CHANNEL_TYPE_SHIFT	0


// channel data type, 8bits
enum{
	TEAPOT_CHANNEL_DATA_TYPE_CMD_TEST = 0x00,
	TEAPOT_CHANNEL_DATA_TYPE_READ_TEST,
	TEAPOT_CHANNEL_DATA_TYPE_WRITE_TEST,
	TEAPOT_CHANNEL_DATA_TYPE_RW_TEST,
	TEAPOT_CHANNEL_DATA_TYPE_MAX
};

#define TEAPOT_CHANNEL_DATA_TYPE_MASK	((1<<TEAPOT_CHANNEL_DATA_TYPE_BITS)-1)
#define TEAPOT_CHANNEL_DATA_TYPE_SHIFT	(TEAPOT_CHANNEL_TYPE_BITS+TEAPOT_CHANNEL_TYPE_SHIFT)


// channel reserve type
#define TEAPOT_CHANNEL_RESERVE_MASK		((1<<TEAPOT_CHANNEL_RESERVE_BITS)-1)
#define TEAPOT_CHANNEL_RESERVE_SHIFT	(TEAPOT_CHANNEL_DATA_TYPE_BITS+TEAPOT_CHANNEL_DATA_TYPE_SHIFT)



#define TEAPOT_CHANNEL_TYPE_SET(type, dtype, reserve) \
	((((type) & TEAPOT_CHANNEL_TYPE_MASK) << TEAPOT_CHANNEL_TYPE_SHIFT) | \
		(((dtype) & TEAPOT_CHANNEL_DATA_TYPE_MASK) << TEAPOT_CHANNEL_DATA_TYPE_SHIFT))

#define TEAPOT_CHANNEL_TYPE_GET(type)	(((type) >> TEAPOT_CHANNEL_TYPE_SHIFT) & TEAPOT_CHANNEL_TYPE_MASK)
#define TEAPOT_CHANNEL_DATA_TYPE_GET(type)	(((type) >> TEAPOT_CHANNEL_DATA_TYPE_SHIFT) & TEAPOT_CHANNEL_DATA_TYPE_MASK)
#define TEAPOT_CHANNEL_RESERVE_GET(type)	(((type) >> TEAPOT_CHANNEL_RESERVE_SHIFT) & TEAPOT_CHANNEL_RESERVE_MASK)


struct teapot_ioctl_data{
	int len;
	void *data;
};

#endif /* TEAPOTSET_INCLUDE_CHANNEL_H_ */
