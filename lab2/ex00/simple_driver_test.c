#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

void	main(void)
{
	int	dev;

	dev = open("/dev/simple_driver_dev", O_RDWR);

	write(dev, "Hello FROM KERNELn\n", 20);

	close(dev);
}
