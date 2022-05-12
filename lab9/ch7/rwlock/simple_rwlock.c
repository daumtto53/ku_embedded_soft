#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/rwlock.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "simple_rwlock_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1	IOCTL_START_NUM + 1
#define IOCTL_NUM2	IOCTL_START_NUM + 2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ	_IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_WRITE	_IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

static dev_t dev_num;
static struct cdev *cd_cdev;
rwlock_t	my_lock;

unsigned long *my_data;

static int ch7_open(struct inode *inode, struct file *file)
{
	printk("open\n");
	return (0);
}

static int ch7_release(struct inode *inode, struct file *file)
{
	printk("release\n");
	return (0);
}


static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	unsigned long *new, *old;

	switch(cmd)
	{
		case IOCTL_READ :
			read_lock_irqsave(&my_lock, flags);
			old = my_data;
			printk("simple_rwlock : Read My_data = %ld\n", *old);
			mdelay(500);
			read_unlock_irqrestore(&my_lock, flags);
			break;
		case IOCTL_WRITE :
			write_lock_irqsave(&my_lock, flags);
			printk("simple_rwlcok : write new data = %ld\n", arg);
			new = (unsigned long *)kmalloc(sizeof(unsigned long), GFP_KERNEL);
			*new = arg;
			old = my_data;
			my_data = new;
			mdelay(200);
			kfree(old);
			write_unlock_irqrestore(&my_lock, flags);
			break;
		default :
			return -1;
	}
	return (0);
}

struct file_operations ch7_fops = {
	.open = ch7_open,
	.release= ch7_release,
	.unlocked_ioctl = simple_ioctl,
};

static int __init ch7_mod_init(void)
{

	printk("init\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch7_fops);
	cdev_add(cd_cdev, dev_num , 1);

	rwlock_init(&my_lock);

	my_data = (unsigned long *)kmalloc(sizeof(unsigned long), GFP_KERNEL);
	*my_data = 0;

	printk("simple_rwlock : init_module\n");

	return (0);
}

static void __exit ch7_mod_exit(void)
{

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
	kfree(my_data);
	printk("simple_rwlock : exit_module\n");
}

module_init(ch7_mod_init);
module_exit(ch7_mod_exit);


