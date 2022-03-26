#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define DEV_NAME "ch2_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4

#define IOCTL_NUM 'z'
#define GET_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SET_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM2, unsigned long)
#define ADD_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM3, unsigned long)
#define MUL_IOCTL _IOWR(IOCTL_NUM, IOCTL_NUM4, unsigned long)

void	main(void)
{
	int	dev_fd;
	long	result = 200;

	dev_fd = open("/dev/ch2_dev", O_RDWR);

	{
		ioctl(dev_fd, SET_IOCTL, 200);

		result = ioctl(dev_fd, GET_IOCTL, NULL);
		printf("get : %ld\n", result);
	}

	{
		ioctl(dev_fd, ADD_IOCTL, 300);

		result = ioctl(dev_fd, GET_IOCTL, NULL);
		printf("add : %ld\n", result);
	}

	{
		ioctl(dev_fd, MUL_IOCTL, 100);

		result = ioctl(dev_fd, GET_IOCTL, NULL);
		printf("mul : %ld\n", result);
	}

	close(dev_fd);
}
