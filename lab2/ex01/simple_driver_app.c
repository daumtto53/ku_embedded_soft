#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define SIMPLE_IOCTL1 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SIMPLE_IOCTL2 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)


int	main(void)
{

	int	dev_f;
	unsigned long arg = 3000;

	dev_f = open("/dev/simple_driver_dev", O_RDWR);
	ioctl(dev_f, SIMPLE_IOCTL1, arg);
	ioctl(dev_f, SIMPLE_IOCTL2, &arg);
	close(dev_f);
	return (0);
}
