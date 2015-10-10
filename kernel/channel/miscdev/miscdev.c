/*
 * miscdev.c
 *
 *  Created on: 2015年4月2日
 *      Author: Administrator
 */

#include <asm-generic/ioctl.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <channel.h>
#include "../channel.h"
#include "miscdev.h"


// 记录teapot miscdev的打开次数
atomic_t teapot_miscdev_opens_num;


static int teapot_miscdev_open(struct inode *inode, struct file *file)
{
	int rc = 0;

	// 对模块进行引用计数
	rc = try_module_get(THIS_MODULE);
	if(!rc){
		rc = -EIO;
		printk("%s: Error attempting to increment module use count; rc = [%d]\n", __func__, rc);
	}
	else{
		rc = 0;
		atomic_inc(&teapot_miscdev_opens_num);
	}
	printk("haha, open\n");

	return rc;
}

static int teapot_miscdev_release(struct inode *inode, struct file *file)
{
	// 释放模块引用，
	module_put(THIS_MODULE);
	atomic_dec(&teapot_miscdev_opens_num);

	printk("hehe, release\n");
	return 0;
}



long teapot_miscdev_ioctl(struct file *filp, unsigned int cmd,
		unsigned long param)
{
	int nr, data_type = _IOC_NR(cmd), size = _IOC_SIZE(cmd);

	printk("ioctl cmd:[%x][%x][%x][%x][%x]\n", cmd, _IOC_DIR(cmd), _IOC_TYPE(cmd), data_type, size);
	printk("ioctl param:%lx\n", param);

	nr = TEAPOT_CHANNEL_TYPE_SET(TEAPOT_CHANNEL_TYPE_IOCTL, data_type, 0);

//	return 0;
	return teapot_channel_transaction(nr, (void *)param, size);
}

static const struct file_operations teapot_miscdev_fops = {
		.owner = THIS_MODULE,
		.open = teapot_miscdev_open,
		.release = teapot_miscdev_release,
		.unlocked_ioctl = teapot_miscdev_ioctl,
};

static struct miscdevice teapot_miscdev = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = "teapot",
		.fops = &teapot_miscdev_fops,
};


static int teapot_ioctl_func(int data_type, void *data, int size)
{
	printk("unit:%p, size:%d\n", data, size);

	if(data_type == TEAPOT_CHANNEL_DATA_TYPE_CMD_TEST && size == sizeof(struct teapot_ioctl_data))
	{
		struct teapot_ioctl_data unit;
		void *tmp;

		if(copy_from_user(&unit, data, size)){
			printk("read data header error\n");
			return -EFAULT;
		}

		printk("data:%p, size:%d\n", unit.data, unit.len);
		tmp = unit.data;
		if(unit.len > PAGE_SIZE)
			unit.data = vmalloc(unit.len);
		else
			unit.data = kmalloc(unit.len, GFP_KERNEL);

		if(!unit.data)
			return -ENOMEM;

		if(copy_from_user(unit.data, tmp, unit.len)){
			printk("read data error\n");
			return -EFAULT;
		}

		printk("unit.data:%s\n", unit.data);
	}

	return -1;
}

int miscdev_init(void)
{
	int rc;

	rc = misc_register(&teapot_miscdev);
	if(rc)
		goto out;

	rc = teapot_channel_register(TEAPOT_CHANNEL_TYPE_IOCTL, TEAPOT_CHANNEL_DATA_TYPE_CMD_TEST, teapot_ioctl_func);

out:
	return rc;

}

void miscdev_exit(void)
{
	misc_deregister(&teapot_miscdev);
	teapot_channel_unregister(TEAPOT_CHANNEL_TYPE_IOCTL, TEAPOT_CHANNEL_DATA_TYPE_CMD_TEST);
}
