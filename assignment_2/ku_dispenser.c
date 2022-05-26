# include <linux/init.h>
# include <linux/kernel.h>
# include <linux/module.h>
# include <linux/errno.h>
# include <linux/types.h>
# include <linux/fs.h>
# include <linux/fcntl.h>
# include <linux/uaccess.h>
# include <linux/gpio.h>
# include <linux/delay.h>
# include <linux/spinlock.h>
# include <linux/list.h>
# include <linux/slab.h>
# include <linux/cdev.h>
# include <linux/wait.h>
# include <linux/sched.h>
# include <linux/uaccess.h>
# include <linux/slab.h>
# include <linux/list.h>
# include <linux/types.h>
# include <linux/jiffies.h>
# include <linux/interrupt.h>

# define TRUE 1
# define FALSE 0

# define DEV_NAME	"ku_dispenser_dev"

# define IOCTL_START_NUM		0x80
# define IOCTL_NUM1	IOCTL_START_NUM + 1
# define IOCTL_NUM2	IOCTL_START_NUM + 2
# define IOCTL_NUM3	IOCTL_START_NUM + 3
# define IOCTL_NUM4	IOCTL_START_NUM + 4

# define KU_DISPENSER_IOCTL_NUM 'z'
# define KU_DISPENSER_GETDATA		_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
# define KU_DISPENSER_CLOSE_OUTLET	_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
# define KU_DISPENSER_OPEN_OUTLET	_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
# define KU_DISPENSER_MAKE_SOUND		_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM4, unsigned long *)

# define PIN1	5
# define PIN2	6
# define PIN3	12
# define PIN4	13
# define STEPS	8
# define ONEROUND 512

# define SPEAKER 16

# define ULTRA_TRIG 23
# define ULTRA_ECHO 24

# define NORMAL_DELAY_MSEC 6 * 10 * 5

MODULE_LICENSE("GPL");

struct ku_dispenser_t
{
	int distance;
	int	is_dispenser_open;
	struct tm timeval;
	long long ktime;
};

struct ku_listnode
{
	struct ku_dispenser_t	*dsp;
	struct list_head		list;
};

struct timer_list timer;

static int irq_num;

static ktime_t echo_start;
static ktime_t echo_stop;
static int		echo_flag = 3;

static struct	ku_listnode head;
static int	list_count = 0;

static dev_t dev_num;
static struct cdev *cd_cdev;

wait_queue_head_t	wq;

static int dispenser_open = 0;

int b[8] = {1, 1, 0, 0, 0, 0, 0, 1};
int p[8] = {0, 1, 1, 1, 0, 0, 0, 0};
int y[8] = {0, 0, 0, 1, 1, 1, 0, 0};
int o[8] = {0, 0, 0, 0, 0, 1, 1, 1};

int notes[] = {1911,1702 ,1516  ,1431  ,1275  , 1136 , 1012 };

void setstep(int p1, int p2, int p3, int p4)
{
	gpio_set_value(PIN1, p1);
	gpio_set_value(PIN3, p3);
	gpio_set_value(PIN4, p4);
}

void backward(int round, int delay)
{
	int i=0;
	int j=0;

	printk("ku_dispenser : fowward\n");
	dispenser_open = TRUE;
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

	printk("ku_dispenser : fowward\n");
	dispenser_open = FALSE;
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

static void send_dispenser_data(unsigned long arg)
{
	struct ku_dispenser_t	*to_send = (struct ku_dispenser_t *)arg;
	struct ku_listnode		*to_send_list;

	if (list_count == 0)
		wait_event_interruptible(wq, list_count > 0);
	list_for_each_entry(to_send_list, &head.list, list)
		break;
	copy_to_user(to_send, to_send_list->dsp, sizeof(struct ku_dispenser_t));
	list_del(&(to_send_list->list));
	list_count--;
	kfree(to_send_list->dsp);
	kfree(to_send_list);
}

static void timer_initiate_sonic(struct timer_list *t)
{
	struct list_head *pos;
	struct list_head *q;
	struct ku_listnode *tmp;
	//real_time. So Over buffer -> delete.
	if (list_count > 1000)
	{
		list_for_each_safe(pos, q, &head.list)
		{
			tmp = list_entry(pos, struct ku_listnode, list);
			list_del(pos);
			kfree(tmp->dsp);
			kfree(tmp);
		}
	}

	if (echo_flag == 3)
	{
		echo_start = ktime_set(0, 1);
		echo_stop = ktime_set(0, 1);
		echo_flag = 0;

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

	struct ku_listnode *tmp = 0;
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
				printk("KERNEL ATOMIC KMALLOC ERROR\n");
				return (IRQ_HANDLED);
			}
			dsp = (struct ku_dispenser_t *)kmalloc(sizeof(struct ku_dispenser_t), GFP_ATOMIC);
			if (!dsp)
			{
				printk("KERNEL ATOMIC KMALLOC ERROR\n");
				return (IRQ_HANDLED);
			}
			tmp->dsp = dsp;
			dsp->ktime = time;
			dsp->distance = cm;
			dsp->is_dispenser_open = dispenser_open;
			dsp->timeval = tm_val;

			list_add_tail(&tmp->list, &head.list);
			list_count++;

			//Timer 가동
			if (dispenser_open)
				mod_timer(&timer, jiffies + msecs_to_jiffies(1 * 1000 / 2));
			else
				mod_timer(&timer, jiffies + msecs_to_jiffies(1 * 1000 * 5 * 60));
		}
	}
	return IRQ_HANDLED;
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

static int ku_dispenser_open(struct inode *inode, struct file *file)
{
	printk("ku_dispenser : open()\n");
	return (0);
}

static int ku_dispenser_release(struct inode *inode, struct file *file) {
	printk("ku_dispenser : close()\n");
	return (0);
}


static long ku_dispenser_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("ku_dispenser : ioctl()\n")
	switch (cmd)
	{
		case KU_DISPENSER_GETDATA:
			send_dispenser_data(arg);
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

struct file_operations ku_dispenser_fops = {
	.open = ku_dispenser_open,
	.release= ku_dispenser_release,
	.unlocked_ioctl = ku_dispenser_ioctl,
};


static int __init ku_dispenser_init(void)
{

	int ret;

	printk("ku_dispenser : init()\n");
	INIT_LIST_HEAD(&head.list);
	init_waitqueue_head(&wq);

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_dispenser_fops);
	cdev_add(cd_cdev, dev_num , 1);

	//sonic_irq
	gpio_request_one(ULTRA_TRIG, GPIOF_OUT_INIT_LOW, "ULTRA_TRIG");
	gpio_request_one(ULTRA_ECHO, GPIOF_IN, "ULTRA_ECHO");
	irq_num = gpio_to_irq(ULTRA_ECHO);
	ret = request_irq(irq_num, ultra_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "ULTRA_ECHO", NULL);
	if (ret)
	{
		printk("ULTRA : unable to request IRQ\n");
		free_irq(irq_num, NULL);
	}
	else
		printk("ULTRA : IRQ request SUCESS\n");


	printk("ku_dispenser : gpio_request_STEPPER_");
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");

	printk("ku_dispenser : gpio_request_SPEAKER");
	gpio_request_one(SPEAKER, GPIOF_OUT_INIT_LOW, "speaker");

	//make timer;
	printk("ku_dispenser : timer init\n");
	timer_setup(&timer, timer_initiate_sonic, 0);
	timer.expires = jiffies + msecs_to_jiffies(60 * 10);

	return (0);
}

static void __exit ku_dispenser_exit(void)
{
	struct ku_listnode		*tmp;
	struct list_head	*pos;
	struct list_head	*q;

	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);
	gpio_free(ULTRA_ECHO);
	gpio_free(ULTRA_TRIG);

	gpio_set_value(SPEAKER, 0);
	gpio_free(SPEAKER);

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);

	list_for_each_safe(pos, q, &head.list)
	{
		tmp = list_entry(pos, struct ku_listnode, list);
		list_del(pos);
		kfree(tmp->dsp);
		kfree(tmp);
	}

	del_timer(&timer);
	free_irq(irq_num, NULL);

}

module_init(ku_dispenser_init);
module_exit(ku_dispenser_exit);
