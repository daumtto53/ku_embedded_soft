#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "simple_driver_dev"

#define SIMPLE_IOCTL_START 0x80
#define IOCTL_NUM1 SIMPLE_IOCTL_START+1
#define IOCTL_NUM2 SIMPLE_IOCTL_START+2

#define SIMPLE_IOCTL_NUM	'z'
#define SIMPLE_IOCTL1 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SIMPLE_IOCTL2 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

static int	simple_ioctl_open(struct inode *inode, struct file *file)
{
	printk("SIMPLE_IOCTL : open()\n");
	return (0);
}


static int	simple_ioctl_release(struct inode *inode, struct file *file)
{
	printk("SIMPLE_IOCTL : close()\n");
	return (0);
}


static ssize_t	simple_ioctl_write(struct file *file, const char *buf, size_t len, loff_t *lof)
{
	printk("SIMPLE_IOCTL : write %s\n", buf);
	return (0);
}


static long	simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
		case SIMPLE_IOCTL1 :
			printk("SIMPLE_IOCTL : ioctl 1 %ld\n", arg);
			break;
		case SIMPLE_IOCTL2 :
			printk("SIMPLE_IOCTL : ioctl 2 %p\n", (unsigned long *)arg);
			break;
		default :
			return (-1);
	}
	return (0);
}

struct file_operations simple_driver_fops = {
	.open = simple_ioctl_open, 
	.release = simple_ioctl_release,
	.write = simple_ioctl_write,
	.unlocked_ioctl = simple_ioctl,
};


static dev_t	dev_num;
static struct cdev *cd_cdev;

static int __init simple_driver_init(void)
{
	printk("SIMPLE_DRIVER : init()\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_driver_fops);
	cdev_add(cd_cdev, dev_num, 1);
	return (0);
}

static void __exit	simple_driver_exit(void)
{
	printk("simple_driver : exit()\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(simple_driver_init);
module_exit(simple_driver_exit);


