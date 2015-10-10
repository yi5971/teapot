/*
 * hook.c
 *
 *  Created on: 2015Äê10ÔÂ10ÈÕ
 *      Author: Administrator
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define PROC_HOME "/proc/kallsyms"
#define READ_BUF_LEN		1024

int hook_search_ksym(const char * sym_name, unsigned long *sym_addr)
{
	mm_segment_t old_fs;
	ssize_t bytes;
	struct file *file = NULL;
	char *p;
	char *read_buf;
	char* bufferpos;
	int rc = 0;
	int leftlen = 0;
	int symlen;
	if (!sym_name)
		return -1;

	file = filp_open(PROC_HOME, O_RDONLY, 0);
	if (!file)
		return -1;

	if (!file->f_op->read)
	{
		rc = -1;
		goto out;
	}
	read_buf = kmalloc(READ_BUF_LEN, GFP_KERNEL);
	bufferpos = read_buf;
	if (!read_buf)
	{
		rc = -ENOMEM;
		goto out;
	}

	symlen = strlen(sym_name);

	old_fs = get_fs();
	set_fs(get_ds());

	while ((bytes = file->f_op->read(file, bufferpos,
			READ_BUF_LEN - 1 - leftlen, &file->f_pos)) > 0)
	{
		char *left;
		read_buf[leftlen + bytes] = 0;
		left = strrchr(read_buf, '\n');
		if (left > 0)
			*left++ = 0;
		else
			left = read_buf + leftlen + bytes;
		bufferpos = read_buf;
		for (; (p = strstr(bufferpos, sym_name)) != NULL;
				bufferpos = p + symlen)
		{
			if (*(p - 1) != ' ')
				continue;
			if (*(p + symlen) != 0 && *(p + symlen) != '\n'
					&& *(p + symlen) != '\t')
				continue;

			p -= sizeof(void *) * 2 + 3;
			p[sizeof(void *) * 2] = '\0';
			*sym_addr = simple_strtoul(p, NULL, 16);
			goto outok;
		}

		leftlen = strlen(left);
		memcpy(read_buf, left, leftlen);
		bufferpos = read_buf + leftlen;
		//printk("diff = %d\n",leftlen);
		//memset (bufferpos, 0, bytes);
	}
	//not found
	rc = -1;
	outok: set_fs(old_fs);
	kfree(read_buf);
	out:

	filp_close(file, NULL);

	return rc;
}
EXPORT_SYMBOL(hook_search_ksym);
