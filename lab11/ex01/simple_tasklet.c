#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

#define SWITCH 17

struct my_data
{
	int year;
	int month;
	int day;
};

struct my_data my_tasklet_data;
struct tasklet_struct my_tasklet;
static int irq_num;

void simple_tasklet_func(unsigned long recv_data) {
	struct my_data *tmp_data;
	tmp_data = (struct my_data *) recv_data;
	printk("today is %d %d %d\n", tmp_data->year, tmp_data->month, tmp_data->day);
}

static irqreturn_t simple_tasklet_isr(int irq, void *data)
{
	printk("simple_tasklet: Interrupt isr\n");
	tasklet_schedule(&my_tasklet);

	return (IRQ_HANDLED);
}

static int __init simple_tasklet_init(void)
{
	int ret;
	printk("simple_tasklet : Init_Moudle\n");

	my_tasklet_data.year = 2022;
	my_tasklet_data.month= 05;
	my_tasklet_data.day= 11;

	tasklet_init(&my_tasklet, simple_tasklet_func, (unsigned long)&my_tasklet_data);

	gpio_request_one(SWITCH, GPIOF_IN, "SWITCH");
	irq_num = gpio_to_irq(SWITCH);
	ret = request_irq(irq_num, simple_tasklet_isr, IRQF_TRIGGER_FALLING, "switch_irq", NULL);
	return (0);
}

static void __exit simple_tasklet_exit(void)
{
	tasklet_kill(&my_tasklet);
	free_irq(irq_num, NULL);
	gpio_free(SWITCH);

	printk("simple_tasklet : EXIT Module\n");
}

module_init(simple_tasklet_init);
module_exit(simple_tasklet_exit);
