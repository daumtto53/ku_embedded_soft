#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include <ku_ipc.h>

//define custom MARCROS
#define DEV_NAME			"ku_ipc_dev"

#define PRINTMOD(str)		printk("KU_IPC : %s\n", str)
#define MSG_LEN				128
#define MAX_ENTRY			10

#define IOCTL_START_NUM		0x80
#define IOCTL_NUM1	IOCTL_START_NUM + 1
#define IOCTL_NUM2	IOCTL_START_NUM + 2
#define IOCTL_NUM3	IOCTL_START_NUM + 3
#define IOCTL_NUM4	IOCTL_START_NUM + 4

#define KU_IPC_IOCTL_NUM 'z'
#define KU_IPC_MSGGET	_IOWR(KU_IPC_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define KU_IPC_MSGCLOSE	_IOWR(KU_IPC_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define KU_IPC_MSGSND	_IOWR(KU_IPC_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
#define KU_IPC_MSGRCV	_IOWR(KU_IPC_IOCTL_NUM, IOCTL_NUM4, unsigned long *)

struct ku_msgbuf
{
	long type;
	char text[MSG_LEN];
};

struct ku_listnode
{
	struct ku_msgbuf	msg;
	struct list_head	*list;
};

struct msgq_wrapper
{
	int					msgq_num[MAX_ENTRY];
	int					msgq_bytes[MAX_ENTRY];
	struct list_head	*msgq_entry[MAX_ENTRY];
	struct list_head	*msgq_user_head[MAX_ENTRY];
};

struct	msgq_metadata
{
	int		msqid;
	void	*msgp;
	int		msgsz;
	long	msgtyp;
	int		msgflg;
};

//spinlock, wq


MODULE_LICENSE("GPL");

static int ku_ipc_open(struct inode *inode, struct file *file)
{
	PRINTMOD("open");
	return (0);
}

static int ku_ipc_release(struct inode *inode, struct file *file)
{
	PRINTMOD("release");
	return (0);
}

static int ku_ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

}

struct file_operations ku_ipc_fops = {
	.open = ku_ipc_open,
	.release = ku_ipc_release,
	.unlocked_ioctl = ku_ipc_ioctl,
};

static dev_t		dev_num;
static struct cdev	*cd_cdev;

static void		init_cdev(void)
{
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_ipc_fops);
	cdev_add(cd_cdev, dev_num, 1);
}


int		__init my_init(void)
{
	PRINTMOD("my_init");
	return (0);
}

void	__exit my_exit(void)
{
	PRINTMOD("my_exit");
}

module_init(my_init);
module_exit(my_exit);
