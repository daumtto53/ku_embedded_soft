#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ch3.h"

int main(void)
{
	int dev_fd;
	struct msg_st	user_msg_st;

	dev_fd = open("/dev/ch3_dev", O_RDWR);
	ioctl(dev_fd, CH3_IOCTL_READ, &user_msg_st);

	printf("READ FROM KERNEL DCLL : [%s]\n", user_msg_st.str);


	ioctl(dev_fd, CH3_IOCTL_READ, &user_msg_st);

	printf("READ FROM KERNEL DCLL : [%s]\n", user_msg_st.str);

	ioctl(dev_fd, CH3_IOCTL_READ, &user_msg_st);

	printf("READ FROM KERNEL DCLL : [%s]\n", user_msg_st.str);


	ioctl(dev_fd, CH3_IOCTL_READ, &user_msg_st);
	

	printf("READ FROM KERNEL DCLL : [%s], len:[%d]\n", user_msg_st.str, user_msg_st.len);
	close(dev_fd);
	return (0);
}
