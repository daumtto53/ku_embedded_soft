#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

int main()
{
	int dev;

	dev = open("/dev/ch6_mod_dev", O_RDWR);
	sleep(60);
	close(dev);
}
