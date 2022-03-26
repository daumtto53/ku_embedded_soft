#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>

#define DEV_NAME "simple_param_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define IOCTL_NUM	'z'
#define PARAM_GET	_IOWR(IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define PARAM_SET	_IOWR(IOCTL_NUM, IOCTL_NUM2, unsigned long)

void	main(void)
{
	int	dev_fd;
	long	my_id;

	dev_fd = open("/dev/simple_param_dev", O_RDWR);

	my_id = 201512285;
	ioctl(dev_fd, PARAM_SET, (unsigned long)my_id);

	my_id = ioctl(dev_fd, PARAM_GET, NULL);
	printf("my_id is %ld \n", my_id);

	close(dev_fd);
}
