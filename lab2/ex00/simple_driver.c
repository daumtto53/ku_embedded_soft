#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "simple_driver_dev"

static int simple_driver_open(struct inode *inode, struct file *file)
{
	printk("simple_driver : OPEN()\n");
	return (0);
}


static int simple_driver_release(struct inode *inode, struct file *file)
{
	printk("simple_driver : CLOSE()\n");
	return (0);
}

static ssize_t	simple_driver_read(struct file *file, char *buf, size_t len, loff_t *lof)
{
	printk("simple_driver : read()\n");
	return (0);
}

static ssize_t	simple_driver_write(struct file *file, const char *buf, size_t len, loff_t *lof)
{

	printk(KERN_ALERT "simple_driver : write() : %s\n", buf);
	return (len);
}

struct file_operations simple_device_fops = {
	.open = simple_driver_open,
	.release = simple_driver_release,
	.write = simple_driver_write,
	.read = simple_driver_read,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_driver_init(void)
{
	printk("simple_driver : init()\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); 
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_device_fops);
	cdev_add(cd_cdev, dev_num, 1);
	return (0);
}

static void __exit simple_driver_exit(void)
{
	printk("simple_driver : exit()\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}


module_init(simple_driver_init);
module_exit(simple_driver_exit);

