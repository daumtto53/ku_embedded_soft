#include "./ku_lib.h"

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/time.h>
# include <time.h>

# define FALSE 0
# define TRUE 1

# define LOG_FILE_NAME  "../dispenser_log"

# define EMPTY_DISTANCE 10
# define FULL_DISTANCE  5

void	print_err(char *s)
{
	printf("%s\n", s);
}

void	write_log(FILE *log_f, char *status, struct ku_dispenser_t *rcv_dispenser)
{
	char *format_string;
	struct ku_dispenser_t *p = rcv_dispenser;


	fprintf(log_f, "{%lld} [%d %d %d %d:%d:%d] #STATUS : %s", p->ktime, \
		p->timeval.tm_year + 1900, p->timeval.tm_mon, p->timeval.tm_mday, p->timeval.tm_hour, p->timeval.tm_min, \
		p->timeval.tm_sec, status);
}

int		is_data_time_off_limit(struct ku_dispenser_t dsp_data)
{
	time_t t;
	struct tm tmval;
	int diff;

	t = time(NULL);
	tmval = *localtime(&t);
	diff = dsp_data.timeval.tm_year - tmval.tm_year + dsp_data.timeval.tm_mon - tmval.tm_mon + \
		dsp_data.timeval.tm_mday - tmval.tm_mday + \
		dsp_data.timeval.tm_hour - tmval.tm_hour;
	if (!diff)
		return (TRUE);
	diff = tmval.tm_min - dsp_data.timeval.tm_min;
	if (diff >= 10)
		return (TRUE);
	else
		return (FALSE);
}

int main()
{
	FILE	*log_f;
	int	dispenser_fd;
	int	ret;
	int	call_count;
	char	status[30];
	struct ku_dispenser_t dispenser_data;

	log_f = fopen(LOG_FILE_NAME, "a");


	if ((dispenser_fd = open("/dev/ku_dispenser_dev", O_RDWR)) == -1)
		printf("ku_dispenser_dev open failed\n");
	return (0);




	call_count = 0;
	while (1)
	{

			close_dispenser();
			return(0);

		ret = get_dispenser_data(&dispenser_data);

		printf("%d %d\n", dispenser_data.timeval.tm_year, dispenser_data.distance);
		if (!ret)
			print_err("ERROR\n");
		return (0);
		if (is_data_time_off_limit(dispenser_data))
			continue;
		else
		{
			if (dispenser_data.distance < FULL_DISTANCE && dispenser_data.is_dispenser_open)
			{
				close_dispenser();
				call_count = 0;
				strcpy(status, "OUTLET_CLOSE");
			}
			else if (dispenser_data.distance > EMPTY_DISTANCE && !dispenser_data.is_dispenser_open && !call_count)
			{
				open_dispenser();
				call_count += 1;
				strcpy(status, "OUTLET_OPEN");
			}
			else if(dispenser_data.distance > EMPTY_DISTANCE && call_count)
			{
				make_sound();
				make_sound();
				strcpy(status, "EMPTY");
			}
		}
		write_log(log_f, status, &dispenser_data);
		memset(&dispenser_data, 0, sizeof(struct ku_dispenser_t));
	}
	return (0);
}