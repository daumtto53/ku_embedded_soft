#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "simple_spin.h"

int		main(void)
{
	int	dev_fd;

	struct str_st user_str = {
		128, "KERNEL IS THE READER : hi KERNEL from USER\n"
	};

	user_str.len = strlen(user_str.str);

	printf("WRITER : hello reader!\n");
	dev_fd = open("/dev/simple_spin_dev", O_RDWR);
	ioctl(dev_fd, SIMPLE_SPIN_WRITE, &user_str);

	close(dev_fd);
}
