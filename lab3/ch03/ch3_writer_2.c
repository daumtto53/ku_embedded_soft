#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ch3.h"

int     main(void)
{       
        int     dev_fd;
        struct msg_st   user_msg_st;
        char    *writer_2_msg = "Hi from writer2!";

        strcpy(user_msg_st.str, writer_2_msg);
        user_msg_st.len = strlen(writer_2_msg);

        dev_fd = open("/dev/ch3_dev", O_RDWR);

        ioctl(dev_fd, CH3_IOCTL_WRITE, &user_msg_st);

        close(dev_fd);
        return (0);
}
