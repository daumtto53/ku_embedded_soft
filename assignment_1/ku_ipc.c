#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "ku_ipc.h"

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
	struct ku_msgbuf	*msg;
	struct list_head	list;
};

struct ku_pid_listnode
{
	int					pid;
	struct list_head	list;
};

struct msgq_wrapper
{
	int						msgq_ref_count[MAX_ENTRY];
	int						msgq_bytes[MAX_ENTRY];
	int						msgq_num[MAX_ENTRY];
	struct ku_listnode		msgq_entry[MAX_ENTRY];
	struct ku_pid_listnode	msgq_entry_pid[MAX_ENTRY];
};

struct	msgq_metadata
{
	int		msqid;
	void	*msgp;
	int		msgsz;
	long	msgtyp;
	int		msgflg;
};

spinlock_t			msgq_lock[MAX_ENTRY];
wait_queue_head_t	ku_wq;
struct msgq_wrapper	msgq_wrap;
//spinlock, wq


MODULE_LICENSE("GPL");

static int ku_ipc_open(struct inode *inode, struct file *file)
{
	PRINTMOD("open");
	return (0);
}

static int ku_ipc_release(struct inode *inode, struct file *file) {
	PRINTMOD("release");
	return (0);
}

static int is_using_msgq(int msgid, int pid)
{
	struct ku_pid_listnode	*node;

	list_for_each_entry(node, &(msgq_wrap.msgq_entry_pid[msgid].list), list)
		if (node->pid == pid)
			return (1);
	return (0);
}

static int add_pid_to_list(int msgid, int pid)
{
	struct ku_pid_listnode *node;

	node = (struct ku_pid_listnode *)kmalloc(sizeof(struct ku_pid_listnode), GFP_KERNEL);
	node->pid = pid;
		printk("add_to_pid : pid=[%d]\n", current->pid);
	list_add_tail(&node->list, &msgq_wrap.msgq_entry_pid[msgid].list);
	return (1);
}

static int ku_ipc_msgget_ioctl(unsigned long arg)
{
	struct msgq_metadata	*meta;
	int						ref_count;
	int						msgid;
	int						msgflg;

	PRINTMOD("ku_ipc_msgget_ioctl");
	meta = (struct msgq_metadata *)kmalloc(sizeof(struct msgq_metadata), GFP_KERNEL);
	copy_from_user(meta, (struct msgq_metadata *)arg, sizeof(struct msgq_metadata));
	msgid = meta->msqid;
	msgflg = meta->msgflg;
	ref_count = msgq_wrap.msgq_ref_count[msgid];
	kfree(meta);

	if (msgid < 0 && msgid > 9)
		return (-1);
	if (is_using_msgq(msgid, current->pid))
		return (-1);

	if (ref_count == 0)
	{
		if (msgflg & KU_IPC_CREAT)
		{
			spin_lock(&msgq_lock[msgid]);
			msgq_wrap.msgq_ref_count[msgid]++;
			add_pid_to_list(msgid, current->pid);
			spin_unlock(&msgq_lock[msgid]);
			return (msgid);
		}
		else if (msgflg == 0)
			return (-1);
	}
	else
	{
		if ((msgflg & KU_IPC_CREAT) && (msgflg & KU_IPC_EXCL))
			return (-1);
		else if ((msgflg == 0) || (msgflg & KU_IPC_CREAT))
		{
			spin_lock(&msgq_lock[msgid]);
			msgq_wrap.msgq_ref_count[msgid]++;
			add_pid_to_list(msgid, current->pid);
			spin_unlock(&msgq_lock[msgid]);
			return (msgid);
		}
	}
	return (-1);
}

static int remove_pid_from_list(int msgid, int pid)
{
	struct ku_pid_listnode	*node;

	list_for_each_entry(node, &(msgq_wrap.msgq_entry_pid[msgid].list), list)
		if (node->pid == pid)
			break;
	spin_lock(&msgq_lock[msgid]);
	list_del(&node->list);
	msgq_wrap.msgq_ref_count[msgid]--;
	spin_unlock(&msgq_lock[msgid]);
	kfree(node);
	return (0);
}

static int ku_ipc_msgclose_ioctl(unsigned long arg)
{
	struct msgq_metadata	*meta;
	int						ref_count;
	int		msgid;

	PRINTMOD("ku_ipc_msgclose_ioctl");
	meta = (struct msgq_metadata *)kmalloc(sizeof(struct msgq_metadata), GFP_KERNEL);
	copy_from_user(meta, (struct msgq_metadata *)arg, sizeof(struct msgq_metadata));
	msgid = meta->msqid;
	ref_count = msgq_wrap.msgq_ref_count[msgid];
	kfree(meta);

	if (msgid < 0 && msgid > 9)
		return (-1);
	if (!is_using_msgq(msgid, current->pid))
		return (-1);

	if (ref_count == 0)
		return (-1);
	else
	{
		remove_pid_from_list(msgid, current->pid);
		return (0);
	}

return (0);
}

static int	is_blocked_condition(int msgid, int msgsz)
{
	return (msgsz + msgq_wrap.msgq_bytes[msgid] > KUIPC_MAXVOL) || (1 + msgq_wrap.msgq_num[msgid] > KUIPC_MAXMSG);
}

static int ku_ipc_msgsnd_ioctl(unsigned long arg)
{
	struct msgq_metadata	*meta;
	struct ku_listnode		*new_node;
	struct ku_msgbuf		*new_msg;
	int						ref_count;
	int		msgid;
	int		msgsz;
	int		msgflg;
	void	*msgp;

	PRINTMOD("ku_ipc_msgsnd_ioctl");
	meta = (struct msgq_metadata *)kmalloc(sizeof(struct msgq_metadata), GFP_KERNEL);
	copy_from_user(meta, (struct msgq_metadata *)arg, sizeof(struct msgq_metadata));
	msgid = meta->msqid;
	msgsz = meta->msgsz;
	msgflg = meta->msgflg;
	msgp = meta->msgp;
	ref_count = msgq_wrap.msgq_ref_count[msgid];
	kfree(meta);

	if (msgid < 0 && msgid > 9)
		return (-1);
	if (ref_count == 0)
		return (-1);
	if (!is_using_msgq(msgid, current->pid))
		return (-1);

	if (is_blocked_condition(msgid, msgsz))
	{
		if (msgflg & KU_IPC_NOWAIT)
			return (-1);
		else
		{
			printk("KU_IPC_MSGSND_IOCTL: WAIT\n");
			wait_event_interruptible(ku_wq, !is_blocked_condition(msgid, msgsz));
		}
	}

	new_node = (struct ku_listnode *)kmalloc(sizeof(struct ku_listnode), GFP_KERNEL);
	new_msg = (struct ku_msgbuf *)kmalloc(sizeof(struct ku_msgbuf), GFP_KERNEL);
	copy_from_user(new_msg, (struct ku_msgbuf *)msgp, sizeof(struct ku_msgbuf));
	new_node->msg = new_msg;
	new_node->msg->type = new_msg->type;
	spin_lock(&msgq_lock[msgid]);
	list_add_tail(&new_node->list, &msgq_wrap.msgq_entry[msgid].list);
	msgq_wrap.msgq_bytes[msgid] += msgsz;
	msgq_wrap.msgq_num[msgid]++;
	spin_unlock(&msgq_lock[msgid]);
	wake_up_interruptible(&ku_wq);
	printk("MSGSND PRINT : qnum:[%d], qbytes:[%d]\n",msgq_wrap.msgq_num[msgid], msgq_wrap.msgq_bytes[msgid]);
	return (msgsz);
}


static int ku_ipc_msgrcv_ioctl(unsigned long arg)
{
	struct msgq_metadata	*meta;
	struct ku_listnode		*node;
	struct ku_listnode		*aux;
	int						ref_count;
	long		msgtyp;
	int		msgid;
	int		msgsz;
	int		original_msgsz;
	int		msgflg;
	void	*msgp;
	char	tmp[MSG_LEN];

	PRINTMOD("ku_ipc_msgrcv_ioctl");
	meta = (struct msgq_metadata *)kmalloc(sizeof(struct msgq_metadata), GFP_KERNEL);
	copy_from_user(meta, (struct msgq_metadata *)arg, sizeof(struct msgq_metadata));
	msgid = meta->msqid;
	msgsz = meta->msgsz;
	original_msgsz = msgsz;
	msgflg = meta->msgflg;
	msgp = meta->msgp;
	msgtyp = meta->msgtyp;
	ref_count = msgq_wrap.msgq_ref_count[msgid];
	kfree(meta);

	node = NULL;
	aux = NULL;

	if (msgid < 0 && msgid > 9)
		return (-1);
	if (!is_using_msgq(msgid, current->pid))
		return (-1);

	if (msgflg & KU_MSG_NOERROR)
	{
		if (msgsz > MSG_LEN)
			msgsz = MSG_LEN;
		else
			return (-1);
	}
	
	if (msgtyp == 0)
	{
		if (msgq_wrap.msgq_num[msgid] == 0)
		{
			if (msgflg & KU_IPC_NOWAIT)
			{
				printk("NOWAIT\n");
				return (-1);
			}
			else
			{
				printk("WATIT\n");
				wait_event_interruptible(ku_wq, msgq_wrap.msgq_num[msgid] > 0);
			}
		}

		list_for_each_entry(node, &msgq_wrap.msgq_entry[msgid].list, list)
			break;
	}
	else if (msgtyp > 0)
	{
		if ((msgq_wrap.msgq_num[msgid] == 0) && (msgflg & KU_IPC_NOWAIT))
			return (-1);
		while (node == NULL)
		{
			list_for_each_entry(aux, &msgq_wrap.msgq_entry[msgid].list, list)
				if(aux->msg->type == msgtyp)
				{
					node = aux;
					break;
				}
			if (node == NULL && (msgflg & KU_IPC_NOWAIT))
				return (-1);
			else if (node == NULL)
				wait_event_interruptible(ku_wq, msgq_wrap.msgq_num[msgid] > 0);
		}
	}
	else
	{
		msgtyp *= -1;
		if ((msgq_wrap.msgq_num[msgid] == 0) && (msgflg & KU_IPC_NOWAIT))
			return (-1);
		while (node == NULL)
		{
			list_for_each_entry(aux, &msgq_wrap.msgq_entry[msgid].list, list)
				if(aux->msg->type <= msgtyp)
				{
					node = aux;
					break;
				}
			if (node == NULL && (msgflg & KU_IPC_NOWAIT))
				return (-1);
			else if (node == NULL)
				wait_event_interruptible(ku_wq, msgq_wrap.msgq_num[msgid] > 0);
		}
	}
	memcpy(tmp, node->msg->text, msgsz);
	memcpy(node->msg->text, tmp, msgsz);
	copy_to_user(msgp, node->msg, sizeof(struct ku_msgbuf));
	spin_lock(&msgq_lock[msgid]);
	msgq_wrap.msgq_bytes[msgid] -= original_msgsz;
	msgq_wrap.msgq_num[msgid]--;
	list_del(&node->list);
	spin_unlock(&msgq_lock[msgid]);
	wake_up_interruptible(&ku_wq);
	printk("RCV_IOCTL1 : msgtyp:[%ld], mtext:[%s]\n", node->msg->type, node->msg->text);
	printk("RCV_IOCTL2 : qnum:[%d], qbytes:[%d]\n",msgq_wrap.msgq_num[msgid], msgq_wrap.msgq_bytes[msgid]);
	kfree(node->msg);
	kfree(node);
	return (msgsz);
}







static long ku_ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int		ret_val;

	switch (cmd)
	{
		case KU_IPC_MSGGET :
			PRINTMOD("KU_IPC_MSGGET");
			ret_val = ku_ipc_msgget_ioctl(arg);
			return (ret_val);

		case KU_IPC_MSGCLOSE :
			PRINTMOD("KU_IPC_MSGCLOSE");
			ret_val = ku_ipc_msgclose_ioctl(arg);
			return (ret_val);

		case KU_IPC_MSGSND :
			PRINTMOD("KU_IPC_MSGSND");
			ret_val = ku_ipc_msgsnd_ioctl(arg);
			return (ret_val);

		case KU_IPC_MSGRCV :
			PRINTMOD("KU_IPC_MSGRCV");
			ret_val = ku_ipc_msgrcv_ioctl(arg);
			return (ret_val);
	}
	return (-1);
}








struct file_operations ku_ipc_fops = {
	.open = ku_ipc_open,
	.release = ku_ipc_release,
	.unlocked_ioctl = ku_ipc_ioctl,
};

static dev_t		dev_num;
static struct cdev	*cd_cdev;

/*
list, spinlock, waitqueue, cdev, msgq_wrapper
*/

static int		init_cdev(void)
{
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_ipc_fops);
	return (cdev_add(cd_cdev, dev_num, 1));
}

static void init_spinlock(void)
{
	int		i;

	for (i = 0; i < MAX_ENTRY; i++)
		spin_lock_init(&msgq_lock[i]);
}

static void init_waitqueue(void)
{
	init_waitqueue_head(&ku_wq);
}

static void		init_msgq_wrapper(void)
{
	int		i;

	for (i = 0; i < MAX_ENTRY; i++)
	{
		msgq_wrap.msgq_ref_count[i] = 0;
		msgq_wrap.msgq_bytes[i] = 0;
		msgq_wrap.msgq_num[i] = 0;
		INIT_LIST_HEAD(&msgq_wrap.msgq_entry[i].list);
		INIT_LIST_HEAD(&msgq_wrap.msgq_entry_pid[i].list);
	}
}

static int		init_datastructure(void)
{
	int		ret;

	PRINTMOD("init_datastructure");
	init_cdev();
	init_spinlock();
	init_waitqueue();
	init_msgq_wrapper();
	ret = 1;
	return (ret);
}

int		__init ku_ipc_init(void)
{
	int		cdev_ret;

	PRINTMOD("---- INIT ------");
	cdev_ret = init_datastructure();
	if (cdev_ret < 0)
	{
		printk("fail to add character device \n");
		return (-1);
	}

	return (0);
}

static void		delete_datastructure(void)
{
	int		i;
	struct ku_listnode		*tmp;
	struct ku_pid_listnode	*pid_tmp;
	struct list_head	*pos;
	struct list_head	*q;


	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);

	for (i = 0 ; i < MAX_ENTRY; i++)
	{
		list_for_each_safe(pos, q, &msgq_wrap.msgq_entry[i].list)
		{
			tmp = list_entry(pos, struct ku_listnode, list);
			list_del(pos);
			kfree(tmp->msg);
			kfree(tmp);
		}
		list_for_each_safe(pos, q, &msgq_wrap.msgq_entry_pid[i].list)
		{
			pid_tmp = list_entry(pos, struct ku_pid_listnode, list);
			list_del(pos);
			kfree(pid_tmp);
		}
	}
}

void	__exit ku_ipc_exit(void)
{
	PRINTMOD("------EXIT-----");
	delete_datastructure();
}

module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
