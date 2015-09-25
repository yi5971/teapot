/*
 * miscdev.c
 *
 *  Created on: 2015年4月2日
 *      Author: Administrator
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
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
	printk("ioctl cmd:[%x][%x]\n", cmd, _IOC_NR(cmd));
	printk("ioctl param:%lx\n", param);

	return 0;
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

int miscdev_init(void)
{
	int rc;

	rc = misc_register(&teapot_miscdev);

	return rc;

}

void miscdev_exit(void)
{
	misc_deregister(&teapot_miscdev);
}
