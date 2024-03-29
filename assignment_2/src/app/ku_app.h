#ifndef __KU_APP_H__
# define __KU_APP_H__

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/time.h>
# include <time.h>

# define FALSE 0
# define TRUE 1

# define LOG_FILE_NAME	"../dispenser_log"
# define DISPENSER_FILE_NAME	"/dev/ku_dispenser_dev"

# define EMPTY_DISTANCE	25.0
# define FULL_DISTANCE	20.0

struct ku_dispenser_t
{
	double distance;
	int	is_dispenser_open;
	struct tm timeval;
	long long ktime;

};

void	print_err(char *s);
void	write_log(int log_fd, char *status, struct ku_dispenser_t *rcv_dispenser);
int		is_data_time_off_limit(struct ku_dispenser_t dsp_data);

#endif
