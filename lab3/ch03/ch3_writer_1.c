#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ch3.h"

int	main(void)
{
	int	dev_fd;
	struct msg_st	user_msg_st;
	char	*writer_1_msg = "Hi from writer1!";
	char	*writer_1_msg_2 = "Hi from writer1! - 22222";
	char	*writer_1_msg_3 = "Hi from writer1! - 33333";

	strcpy(user_msg_st.str, writer_1_msg);
	user_msg_st.len = strlen(writer_1_msg);

	dev_fd = open("/dev/ch3_dev", O_RDWR);
	if (dev_fd < 0)
	{
		printf("OPEN ERROR : fd[%d]\n", dev_fd);
		return (0);
	}
	
	printf("%s", writer_1_msg);
	
	ioctl(dev_fd, CH3_IOCTL_WRITE, &user_msg_st);

	memset(&user_msg_st, 0, sizeof(struct msg_st));
	strcpy(user_msg_st.str, writer_1_msg_2);
	user_msg_st.len = strlen(writer_1_msg_2);

	ioctl(dev_fd, CH3_IOCTL_WRITE, &user_msg_st);

	memset(&user_msg_st, 0, sizeof(struct msg_st));
	strcpy(user_msg_st.str, writer_1_msg_3);
	user_msg_st.len = strlen(writer_1_msg_3);

	ioctl(dev_fd, CH3_IOCTL_WRITE, &user_msg_st);
	
	
	close(dev_fd);
	
	return (0);
}
