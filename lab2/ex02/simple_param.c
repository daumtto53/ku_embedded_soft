#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "simple_param_dev"

#define IOCTL_START	0x80
#define IOCTL_NUM1	IOCTL_START+1
#define IOCTL_NUM2	IOCTL_START+2

#define PARAM_IOCTL_NUM 'z'
#define PARAM_GET	_IOWR(PARAM_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define PARAM_SET	_IOWR(PARAM_IOCTL_NUM, IOCTL_NUM2, unsigned long)

static long my_id = 0;
module_param(my_id, long, 0);

static int simple_param_open(struct inode *inode, struct file *file)
{
	printk("SIMPLE_PARAM : open()\n");
	return (0);
}

static int simple_param_release(struct inode *inode, struct file * file)
{
	printk("SIMPLE_PARAM : release()\n");
	return (0);
}

static long simple_param_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
		case PARAM_GET :
			printk("simple_param returned my_id : %d\n", my_id);
			return (my_id);
		case PARAM_SET :
			printk("simple_param set my_id : %d to %d\n", my_id, arg);
			my_id = (long)arg;
			return (my_id);
		default :
			return (-1);
	}
	return (0);
}

struct file_operations simple_param_fops = {
	.open = simple_param_open,
	.release = simple_param_release,
	.unlocked_ioctl = simple_param_ioctl,
};

static dev_t	dev_num;
static struct cdev	*cd_cdev;

static int __init	simple_param_init(void)
{
	printk("SIMPLE_PARAM : init()\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_param_fops);
	cdev_add(cd_cdev, dev_num, 1);

	return (0);
}

static void __exit	simple_param_exit(void)
{
	printk("SIMPLE_PARAM : exit()\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(simple_param_init);
module_exit(simple_param_exit);
