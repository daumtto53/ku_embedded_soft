#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26

#define STEPS 8
#define ONEROUND 512

int b[8] = {1, 1, 0, 0, 0, 0, 0, 1};
int p[8] = {0, 1, 1, 1, 0, 0, 0, 0};
int y[8] = {0, 0, 0, 1, 1, 1, 0, 0};
int o[8] = {0, 0, 0, 0, 0, 1, 1, 1};

void setstep(int p1, int p2, int p3, int p4)
{
	gpio_set_value(PIN1, p1);
	gpio_set_value(PIN2, p2);
	gpio_set_value(PIN3, p3);
	gpio_set_value(PIN4, p4);
}

void backward(int round, int delay)
{
	int i=0;
	int j=0;

	for (i = 0; i < ONEROUND * round; i++)
	{
		for (j = STEPS; j > 0; j--)
		{
			setstep(b[j], p[j], y[j], o[j]);
			udelay(delay);
		}
	}
	setstep(0,0,0,0);
}



void forward(int round, int delay)
{
	int i=0;
	int j=0;

	for (i = 0; i < ONEROUND * round; i++)
	{
		for (j = 0; j < STEPS; j++)
		{
			setstep(b[j], p[j], y[j], o[j]);
			udelay(delay);
		}
	}
	setstep(0,0,0,0);
}

static int __init simple_motor_init(void)
{
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");

	forward(1, 3000);
	mdelay(3000);

	backward(1, 1500);
	return (0);
}

static void __exit simple_motor_exit(void)
{
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);
}

module_init(simple_motor_init);
module_exit(simple_motor_exit);
