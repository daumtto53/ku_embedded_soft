#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "ch2_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4

#define IOCTL_NUM 'z'
#define GET_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SET_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM2, unsigned long)
#define ADD_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM3, unsigned long)
#define MUL_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM4, unsigned long)


static long result = 0;
module_param(result, long, 0);

static int ch2_driver_open(struct inode *inode, struct file *file)
{
	printk("CH2 : open()\n");
	return (0);
}

static int ch2_driver_release(struct inode *inode, struct file *file)
{
	printk("CH2 : close()\n");
	return (0);
}


static long ch2_driver_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
	switch (cmd)
	{
		case GET_IOCTL :
			printk("CH2 : GET_IOCTL | get result : %ld\n", (long)result);
			return ((long)result);
		case SET_IOCTL :
			printk("CH2 : SET_IOCTL | set result : %ld to %ld\n", result, (long)arg);
			result = (long)arg;
			return (result);
		case ADD_IOCTL :
			printk("CH2 : ADD_IOCTL | add result : %ld + %ld = %ld\n", result, (long)arg, result + (long)arg);
			result += (long)arg;
			return (result);
		case MUL_IOCTL :
			printk("CH2 : MUL_IOCTL | mult result : %ld * %ld = %ld\n", result, (long)arg, result * (long)arg);
			result *= (long)arg;
			return (result);
		default :
			return (-1);
	}
	return (0);
}

struct file_operations	ch2_fops = {
	.open = ch2_driver_open,
	.release = ch2_driver_release,
	.unlocked_ioctl = ch2_driver_ioctl,
};

static dev_t	dev_num;
static struct cdev	*cd_cdev;

static int __init	ch2_driver_init(void)
{
	printk("CH2 : init()\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);

	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch2_fops);
	cdev_add(cd_cdev, dev_num, 1);

	return (0);
}

static void __exit	ch2_driver_exit(void)
{
	printk("CH2 : exit()\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(ch2_driver_init);
module_exit(ch2_driver_exit);
