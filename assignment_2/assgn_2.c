#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define SENSOR 17
#define LED 5
#define DEV_NAME "ch6_mod_dev"

static	int irq_num;
struct	timespec start;
struct	timespec recent;
struct	timer_list timer;
struct	timer_list on_timer;
long	two_sec_delay_jiffies;
long	on_delay_jiffies;

static int ch6_mod_open(struct inode *inode, struct file *file)
{
	printk("open\n");
	getnstimeofday(&start);
	return (0);
}

static int ch6_mod_release(struct inode *inode, struct file *file)
{
	printk("release\n");
	return (0);
}

struct file_operations ch6_fops = {
	.open = ch6_mod_open,
	.release= ch6_mod_release,
};

static irqreturn_t ch6_mod_isr(int irq, void *dev_id)
{
	printk("IRQ\n");

	gpio_set_value(LED, 0);
	mod_timer(&on_timer, jiffies + on_delay_jiffies);
	getnstimeofday(&recent);
	printk("CH6 : Detect at %ld secs\n", recent.tv_sec - start.tv_sec);

	return IRQ_HANDLED;
}

static void timer_func_two_sec_led(struct timer_list *t)
{
	gpio_set_value(LED, 0);
}

static void timer_func_on_led(struct timer_list *t)
{
	gpio_set_value(LED, 1);
	mod_timer(&timer, jiffies + two_sec_delay_jiffies);

}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch6_mod_init(void)
{

	int ret;

	printk("init\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch6_fops);
	cdev_add(cd_cdev, dev_num , 1);

	//gpio_request : ULTRA_TRIG, ULTRA_ECHO
	gpio_request_one(SENSOR, GPIOF_IN, "sensor");
	gpio_request_one(LED, GPIOF_OUT_INIT_LOW, "led");
	irq_num = gpio_to_irq(SENSOR);
	ret = request_irq(irq_num, ch6_mod_isr, IRQF_TRIGGER_RISING, "sensor_irq", NULL);
	if (ret)
	{
		printk("unable to request IRQ\n");
		free_irq(irq_num, NULL);
	}
	else
		printk("IRQ request success\n");
	two_sec_delay_jiffies = msecs_to_jiffies(2000);
	on_delay_jiffies = msecs_to_jiffies(300);
	timer_setup(&timer, timer_func_two_sec_led, 0);
	timer_setup(&on_timer, timer_func_on_led, 0);
	timer.expires = jiffies + msecs_to_jiffies(2000);
	on_timer.expires = jiffies + msecs_to_jiffies(300);

	return (0);
}

static void __exit ch6_mod_exit(void)
{

	printk("exit\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
	free_irq(irq_num, NULL);
	gpio_free(SENSOR);
	gpio_free(LED);
	del_timer(&timer);
	del_timer(&on_timer);
}

module_init(ch6_mod_init);
module_exit(ch6_mod_exit);

