#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>

#define SWITCH 12
#define LED 5

MODULE_LICENSE("GPL");


struct my_timer_info
{
	struct timer_list timer;
	long delay_jiffies;
};

static struct my_timer_info my_timer;

static void my_timer_func(struct timer_list *t)
{

	int ret;
	struct my_timer_info *info = from_timer(info, t ,timer);

	printk("ch05 : timer_func\n");
	ret = gpio_get_value(SWITCH);
	
	if (ret)
	{
		printk("ch5 : set LED, JIFFIES=[%ld]", jiffies);
		gpio_set_value(LED, 1);
	}
	else
	{
		printk("ch5 : off LED, JIFFIES=[%ld]", jiffies);
		printk("off LED");
		gpio_set_value(LED, 0);
	}
	mod_timer(&my_timer.timer, jiffies + info->delay_jiffies);

}

static int __init ch05_mod_init(void)
{
	printk("ch05 : init\n");
	gpio_request_one(LED, GPIOF_OUT_INIT_LOW, 0);
	gpio_request_one(SWITCH, GPIOF_IN, 0);

	my_timer.delay_jiffies = msecs_to_jiffies(500);
	
	timer_setup(&my_timer.timer, my_timer_func, 0);
	my_timer.timer.expires = jiffies + my_timer.delay_jiffies;
	add_timer(&my_timer.timer);
	return (0);
}

static void __exit ch05_mod_exit(void)
{
	printk("simple_timer : exit\n");
	del_timer(&my_timer.timer);
	gpio_free(SWITCH);
	gpio_set_value(LED, 0);
	gpio_free(LED);
}

module_init(ch05_mod_init);
module_exit(ch05_mod_exit);

