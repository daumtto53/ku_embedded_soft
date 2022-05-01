#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "ku_ipc.h"

//ku.h start
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

struct	msgq_metadata
{
	int		msqid;
	void	*msgp;
	int		msgsz;
	long	msgtyp;
	int		msgflg;
};
//ku.h end

int ku_msgget(int key, int msgflg)
{
	int		dev;
	int		msgq_id;
	struct	msgq_metadata	msgq_meta;

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	msgq_meta.msqid = key;
	msgq_meta.msgflg = msgflg;

	msgq_id = ioctl(dev, KU_IPC_MSGGET, &msgq_meta);
	close(dev);
	return (msgq_id);
}

int ku_msgclose(int msqid)
{
	int		dev;
	int		msgclose_ret;
	struct	msgq_metadata	msgq_meta; 

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	msgq_meta.msqid = msqid;

	msgclose_ret = ioctl(dev, KU_IPC_MSGCLOSE, &msgq_meta);
	close(dev);
	return (msgclose_ret);
}

int ku_msgsnd(int msqid, void *msgp, int msgsz, int msgflg)
{
	int dev;
	int msgsnd_ret;
	struct	msgq_metadata	msgq_meta; 

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	msgq_meta.msqid = msqid;
	msgq_meta.msgp = msgp;
	msgq_meta.msgsz = msgsz;
	msgq_meta.msgflg = msgflg;

	msgsnd_ret = ioctl(dev, KU_IPC_MSGSND, &msgq_meta);
	close(dev);
	return (msgsnd_ret);
}

int	ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg)
{
	int dev;
	int msgrcv_ret;
	struct	msgq_metadata	msgq_meta; 

	msgq_meta.msqid = msqid;
	msgq_meta.msgp = msgp;
	msgq_meta.msgsz = msgsz;
	msgq_meta.msgtyp = msgtyp;
	msgq_meta.msgflg = msgflg;

	dev = open("/dev/ku_ipc_dev", O_RDWR);
	msgrcv_ret = ioctl(dev, KU_IPC_MSGRCV, &msgq_meta);
	close(dev);
	return (msgrcv_ret);
}

