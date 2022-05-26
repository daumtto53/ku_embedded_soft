#include "./ku_app.h"

void	print_err(char *s)
{
	printf("%s\n", s);
}

void	write_log(int log_fd, char *status, struct ku_dispenser_t *rcv_dispenser)
{
	char *format_string;
	struct ku_dispenser_t *p = rcv_dispenser;

	fprintf(log_fd, "{%lld} [%d %d %d %d:%d:%d] #STATUS : %s", p->ktime, \
		p->year, p->month, p->day, p->hour, p->min, p->sec, status);
}

int		is_data_time_off_limit(struct ku_dispenser_t dsp_data)
{
	time_t t;
	struct tm tmval;
	int diff;

	t = time(NULL);
	tmval = *localtime(&t);
	diff = dsp_data.year - tmval.tm_year + dsp_data.month - tmval.tm_mon + dsp_data.day - tmval.tm_mday +\
		dsp_data.hour - tmval.tm_hour;
	if (!diff)
		return (TRUE);
	diff = tmval.tm_min - dsp_data.min;
	if (diff >= 10)
		return (TRUE);
	else
		return (FALSE);
}

int main()
{
	int	log_fd;
	int	dispenser_fd;
	int	ret;
	int	call_count;
	char	status[30];
	struct ku_dispenser_t dispenser_data;

	log_fd = open(LOG_FILE_NAME, O_CREAT | O_EXCL | O_RDWR | O_APPEND);

	call_count = 0;
	while (1)
	{
		ret = get_dispenser_data(&dispenser_data);
		if (!ret)
			print_err("ERROR\n");
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
		write_log(log_fd, status, &dispenser_data);
		memset(&dispenser_data, 0, sizeof(struct ku_dispenser_t));
	}
	return (0);
}
