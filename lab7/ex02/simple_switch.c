#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/delay.h>

#define SWITCH 12

MODULE_LICENSE("GPL");

static int count = 0;

static int __init simple_switch_init(void)
{
	int ret = 0;
	printk("simple_switch init\n");

	gpio_request_one(SWITCH, GPIOF_IN, NULL);

	while(count != 5)
	{
		ret = gpio_get_value(SWITCH);
		printk("ret = %d\n", ret);
		if (ret)
		{
			count++;
			printk("Pressed Button : count : %d\n", count);
		}
		msleep(1000);
	}
	printk("simple_switch : finished Pushed Button\n");
	return (0);
}

static void __exit simple_switch_exit(void)
{
	printk("simple_switch exit\n");
	gpio_free(SWITCH);
}

module_init(simple_switch_init);
module_exit(simple_switch_exit);
