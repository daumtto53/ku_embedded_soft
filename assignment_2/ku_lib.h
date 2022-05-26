#ifndef __KU_LIB_H__
# define __KU_LIB_H__

# include <sys/time.h>
# include <time.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/fcntl.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <string.h>
# include <stdlib.h>


# define FALSE 0
# define TRUE 1

# define LOG_FILE_NAME	"../dispenser_log"
# define DISPENSER_FILE_NAME	"/dev/ku_dispenser_dev"

#define IOCTL_START_NUM		0x80
#define IOCTL_NUM1	IOCTL_START_NUM + 1
#define IOCTL_NUM2	IOCTL_START_NUM + 2
#define IOCTL_NUM3	IOCTL_START_NUM + 3
#define IOCTL_NUM4	IOCTL_START_NUM + 4

#define KU_DISPENSER_IOCTL_NUM 'z'
#define KU_DISPENSER_GETDATA		_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define KU_DISPENSER_CLOSE_OUTLET	_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define KU_DISPENSER_OPEN_OUTLET	_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
#define KU_DISPENSER_MAKE_SOUND		_IOWR(KU_DISPENSER_IOCTL_NUM, IOCTL_NUM4, unsigned long *)


struct ku_dispenser_t
{
	int distance;
	int	is_dispenser_open;
	struct tm timeval;
	long long ktime;

};

int  get_dispenser_data(struct ku_dispenser_t *dsp_p);
void close_dispenser();
void open_dispenser();
void make_sound();

#endif
