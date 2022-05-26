#ifndef __KU_DISPENSER_H__
#define __KU_DISPENSER_H__

# include <linux/init.h>
# include <linux/kernel.h>
# include <linux/module.h>
# include <linux/errno.h>
# include <linux/types.h>
# include <linux/fs.h>
# include <linux/fcntl.h>
# include <linux/uaccess.h>
# include <linux/spinlock.h>
# include <linux/list.h>
# include <linux/slab.h>
# include <linux/cdev.h>
# include <linux/wait.h>
# include <linux/sched.h>
# include <linux/uaccess.h>

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

# define NORMAL_DELAY_MSEC 1 * 1000 * 10

struct ku_dispenser_t
{
	double distance;
	int	is_dispenser_open;
	struct tm timeval;
	long long ktime;
};

struct ku_listnode
{
	struct ku_dispenser_t	*dsp;
	struct list_head		list;
};

#endif
