#include "./ku_lib.h"

int get_dispenser_data(struct ku_dispenser_t *dsp)
{
	int	dispenser_fd;
	int	ret;

	dispenser_fd = open(DISPENSER_FILE_NAME, O_RDWR);
	ret = ioctl(dispenser_fd, KU_DISPENSER_GETDATA, dsp);
	close(dispenser_fd);
	return (ret);
}

void close_dispenser()
{
	int	dispenser_fd;
	int	ret;

	dispenser_fd = open(DISPENSER_FILE_NAME, O_RDWR);
	ret = ioctl(dispenser_fd, KU_DISPENSER_CLOSE_OUTLET, NULL);
	close(dispenser_fd);
}

void open_dispenser()
{
	int	dispenser_fd;
	int	ret;

	dispenser_fd = open(DISPENSER_FILE_NAME, O_RDWR);
	ret = ioctl(dispenser_fd, KU_DISPENSER_OPEN_OUTLET, NULL);
	close(dispenser_fd);
}

void make_sound()
{
	int	dispenser_fd;
	int	ret;

	dispenser_fd = open(DISPENSER_FILE_NAME, O_RDWR);
	ret = ioctl(dispenser_fd, KU_DISPENSER_MAKE_SOUND, NULL);
	close(dispenser_fd);
}


