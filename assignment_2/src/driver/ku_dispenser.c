#include "./ku_dispenser.h"

struct timer_list timer;

static int irq_num;
static ktime_t echo_start;
static ktime_t echo_stop;
static int		echo_flag = 3;

static ku_listnode head;

static dev_t dev_num;
static struct cdev *cd_cdev;

static int dispenser_open = 0;

int b[8] = {1, 1, 0, 0, 0, 0, 0, 1};
int p[8] = {0, 1, 1, 1, 0, 0, 0, 0};
int y[8] = {0, 0, 0, 1, 1, 1, 0, 0};
int o[8] = {0, 0, 0, 0, 0, 1, 1, 1};

int notes[] = {1911,1702 ,1516  ,1431  ,1275  , 1136 , 1012 };

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

	for (i = 0; i < ONEROUND/2 * round; i++)
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

	for (i = 0; i < ONEROUND/2 * round; i++)
	{
		for (j = 0; j < STEPS; j++)
		{
			setstep(b[j], p[j], y[j], o[j]);
			udelay(delay);
		}
	}
	setstep(0,0,0,0);
}


static void timer_initiate_sonic(struct timer_list *t)
{
	if (echo_flag == 3)
	{
		echo_start = ktime_set(0, 1);
		echo_stop = ktime_set(0, 1);
		echo_valid_flag = 0;

		gpio_set_value(ULTRA_TRIG, 1);
		udelay(10);
		gpio_set_value(ULTRA_ECHO, 0);
		echo_flag = 1;
	}
}

static irqreturn_t ultra_isr(int irq, void *dev_id)
{
	ktime_t tmp_time;
	long long time;
	int cm;

	struct test_list *tmp = 0;
	struct list_head *pos = 0;
	struct ku_dispenser_t *dsp = 0;

	struct timeval now;
	struct tm tm_val;


	tmp_time = ktime_get();
	if (echo_flag == 1)
	{
		printk("ULTRA : echo up\n");
		if (gpio_get_value(ULTRA_ECHO) == 1)
		{
			echo_start = tmp_time;
			echo_flag = 2;
		}
	}
	else if (echo_flag == 2)
	{
		printk("ULTRA : echo down\n");
		if (gpio_get_value(ULTRA_ECHO) == 0)
		{
			echo_stop = tmp_time;
			time = ktime_to_us(ktime_sub(echo_stop, echo_start));
			cm = (int) time / 58;
			echo_flag = 3;

			do_gettimeofday(&now);
			time_to_tm(now.tv_sec, 0, &tm_val);

			tmp = (struct ku_listnode *)kmalloc(sizeof(struct ku_listnode), GFP_ATOMIC);
			if (!tmp)
			{
				printf("KERNEL ATOMIC KMALLOC ERROR\n");
				return (IRQ_HANDLED);
			}
			dsp = (struct ku_dispenser_t *)kmalloc(sizeof(struct ku_dispenser_t), GFP_ATOMIC);
			if (!dsp)
			{
				printf("KERNEL ATOMIC KMALLOC ERROR\n");
				return (IRQ_HANDLED);
			}
			tmp->dsp = dsp;
			dsp->ktime = time;
			dsp->distance = cm;
			dsp->is_dispenser_open = dispenser_open;
			dsp->timeval = tm_val;

			list_add_tail(&tmp->list, &head->list);
		}
	}
	return IRQ_HANDLED;
}

static int ku_dispenser_open(struct inode *inode, struct file *file)
{
	printk("ku_dispenser : open()\n");
	return (0);
}

static int ku_dispenser_release(struct inode *inode, struct file *file) {
	printk("ku_dispenser : close()\n");
	return (0);
}

//speaker
static void play(int note)
{
	int i = 0;

	for (i=0; i < 100; i++)
	{
		gpio_set_value(SPEAKER, 1);
		udelay(note);
		gpio_set_value(SPEAKER, 0);
		udelay(note);
	}
}


static long ku_dispenser_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int		ret_val;

	switch (cmd)
	{
		case KU_DISPENSER_GET_DATA:
			copy_data_to_user(arg);
			return (0);
		case KU_DISPENSER_CLOSE_OUTLET:
			forward(1, 3000);
			return (0);
		case KU_DISPENSER_OPEN_OUTLET:
			backward(1, 3000);
			return (0);
		case KU_DISPENSER_MAKE_SOUND:
			play(notes[3]);
			mdelay(500);
			gpio_set_value(SPEAKER, 0);
			return (0);

	}
	return (0);
}



static int __init ku_dispenser_init(void)
{

	int ret;

	unsigned int i;
	INIT_LIST_HEAD(&head.list);

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch6_fops);
	cdev_add(cd_cdev, dev_num , 1);

	//sonic_irq
	gpio_request_one(ULTRA_TRIG, GPIOF_OUT_INIT_LOW, "ULTRA_TRIG");
	gpio_request_one(ULTRA_ECHO, GPIOF_IN, "ULTRA_ECHO");
	irq_num = gpio_to_irq(ULTRA_ECHO);
	ret = request_irq(irq_num, ultra_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "ULTRA_ECHO", NULL);
	if (!ret)
	{
		printk("ULTRA : unable to request IRQ\n");
		free_irq(irq_num, NULL);
	}

	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");

	//make timer;
	timer_setup(&timer, timer_initiate_sonic, 0);
	timer.expires = jiffies + msecs_to_jiffies(NORMAL_DELAY_MSEC);

	return (0);
}

static void __exit simple_motor_exit(void)
{
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);

	gpio_set_value(SPEAKER, 0);
	gpio_free(SPEAKER);

}

module_init(simple_motor_init);
module_exit(simple_motor_exit);

