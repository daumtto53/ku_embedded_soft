#include <unistd.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>

#include "simple_spin.h"

int	main(void)
{
	int	dev_fd;

	struct	str_st user_str;
	dev_fd = open("/dev/simple_spin_dev", O_RDWR);
	ioctl(dev_fd, SIMPLE_SPIN_READ, &user_str);
	printf("%s\n", user_str.str);
	close(dev_fd);
	return (0);
}
