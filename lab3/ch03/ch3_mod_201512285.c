#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/fcntl.h>
#include <linux/errno.h>
#include <linux/types.h>

#include "ch3.h"

#define DEV_NAME	"ch3_dev"

spinlock_t	ch3_lock;

struct msg_list
{
	struct list_head	list;
	struct msg_st		msg;
};

static struct msg_list		msg_list_head;

static int	msg_strcpy(char *dst, char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (i);
}

//user ioctl's to read from kernel's msg_list;
//read from head;
//delete from head;
//if empty list : return msg_st /len;0/str;'\0'
//else return .
static int	ch3_ioctl_read(struct msg_st *user_msg_st)
{
	struct msg_list		*first_element;
	struct list_head	*first_list_head;

	printk("CH3 : ioctl_read\n");

	if (list_empty(&msg_list_head.list))
	{
		user_msg_st->len = 0;
		user_msg_st->str[0] = '\0';
		printk("CH3 : ioctl_reading...  [\'0\']\n");
	}
	else
	{
		first_list_head = msg_list_head.list.next;
		first_element = (struct msg_list *)list_entry(first_list_head, struct msg_list, list);
		spin_lock(&ch3_lock);
		user_msg_st->len = first_element->msg.len;
		msg_strcpy(user_msg_st->str, first_element->msg.str);
		spin_unlock(&ch3_lock);
		printk("CH3 : ioctl_reading... [%s]\n", first_element->msg.str);
		list_del_init(first_list_head);
		kfree(first_list_head);
	}

	return (0);
}

//user ioctl's to write to kernel's msg_list
//arg : struct msg_st *user_msg_st
//allocate msg_st , msg_list.
//msg_list = allocated, msg_list.msg_st = msg_st
//list_add_tail(new list_head, list_head)
//
//struct list_head , struct msg_st, strcut msg_list, struct msg_list msg_list_head,
static int	ch3_ioctl_write(struct msg_st *user_msg_st)
{
	struct msg_list	*new_msg_list;
	int	len;

	printk("CH3 : ioctl_write()\n");

	new_msg_list = (struct msg_list *)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	spin_lock(&ch3_lock);
	len = msg_strcpy(new_msg_list->msg.str, user_msg_st->str);

	new_msg_list->msg.len = user_msg_st->len;
	spin_unlock(&ch3_lock);
	list_add_tail(&new_msg_list->list, &msg_list_head.list);
	printk("CH3 : ioctl_writing... [%s]\n", new_msg_list->msg.str);
	return (len);
}

static long	ch3_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct msg_st	*user_msg_st;

	printk("CH3 : ioctl()\n");
	user_msg_st = (struct msg_st *)arg;
	switch (cmd)
	{
		case CH3_IOCTL_WRITE :
			printk("CH3 : CH3_IOCTL_WRITE()\n");
			ch3_ioctl_write(user_msg_st);
			break;
		case CH3_IOCTL_READ :
			printk("CH3 : CH3_IOCTL_READ()\n");
			ch3_ioctl_read(user_msg_st);
			break;
		default :
			return (-1);
	}
	return (0);
}

static int	ch3_open(struct inode *inode, struct file *file)
{
	printk("CH3 : open()\n");
	return (0);
}

static int	ch3_release(struct inode *inode, struct file *file)
{
	printk("CH3 : release()\n");
	return (0);
}

struct file_operations	ch3_fops = {
	.unlocked_ioctl = ch3_ioctl,
	.open = ch3_open,
	.release = ch3_release,
};

static dev_t	dev_num;
struct cdev	*cd_cdev;



static int __init	ch3_init(void)
{
	int	ret;

	printk("CH3 : init() module\n");

	INIT_LIST_HEAD(&msg_list_head.list);
	
	spin_lock_init(&ch3_lock);

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch3_fops);
	ret = cdev_add(cd_cdev, dev_num, 1);
	if (ret < 0)
	{
		printk("fail to add character device\n");
		return (-1);
	}
	return (0);
}

static void __exit	ch3_exit(void)
{
	struct msg_list	*tmp;
	struct list_head *pos;
	struct list_head *q;

	printk("CH3 : exit() module\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
	printk("CH3 : free_list\n");
	list_for_each_safe(pos, q, &msg_list_head.list)
	{
		tmp = list_entry(pos, struct msg_list, list);
		list_del(pos);
		kfree(tmp);
	}
}

module_init(ch3_init);
module_exit(ch3_exit);
