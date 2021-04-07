#include <ncurses.h>
#include <form.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

static int set_interface_attribs(int fd, int speed, int parity)
{
	struct termios tty;
	if(tcgetattr(fd, &tty) != 0)
	{
		fprintf(stderr, "(tcgetattr) Error %d\n", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
	tty.c_iflag &= ~IGNBRK;
	tty.c_lflag = 0;
	tty.c_oflag = 0;
	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 5;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;
	if(tcsetattr(fd, TCSANOW, &tty) != 0)
	{
		fprintf(stderr, "(tcsetattr) Error %d\n", errno);
		return -1;
	}

	return 0;
}

static int set_blocking(int fd, int should_block)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if(tcgetattr(fd, &tty) != 0)
	{
		fprintf(stderr, "(tcgetattr) Error %d\n", errno);
		return -1;
	}

	tty.c_cc[VMIN] = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;
	if(tcsetattr(fd, TCSANOW, &tty) != 0)
	{
		fprintf(stderr, "(tcsetattr) Error %d\n", errno);
		return -1;
	}

	return 0;
}

static int init_fd(char *port, int baud)
{
	int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		printf("(open) Error %d: %s", errno, strerror(errno));
		return -1;
	}

	switch(baud)
	{
		case 300:
			set_interface_attribs(fd, B300, 0);
			break;

		case 600:
			set_interface_attribs(fd, B600, 0);
			break;

		case 1200:
			set_interface_attribs(fd, B1200, 0);
			break;

		case 2400:
			set_interface_attribs(fd, B2400, 0);
			break;

		case 4800:
			set_interface_attribs(fd, B4800, 0);
			break;

		case 9600:
			set_interface_attribs(fd, B9600, 0);
			break;

		case 19200:
			set_interface_attribs(fd, B19200, 0);
			break;

		case 38400:
			set_interface_attribs(fd, B38400, 0);
			break;

		case 57600:
			set_interface_attribs(fd, B57600, 0);
			break;

		case 115200:
			set_interface_attribs(fd, B115200, 0);
			break;
	}

	set_blocking(fd, 1);
	return fd;
}

int kbhit(void)
{
	int ch;
	nodelay(stdscr, TRUE);
	noecho();
	ch = getch();
	echo();
	nodelay(stdscr, FALSE);
	return ch == ERR ? 0 : ch;
}

static char *_trim(char *str, int *len)
{
	char *end;
	while(isspace(*str))
	{
		++str;
	}

	if(*str == 0)
	{
		*len = 0;
		return str;
	}

	end = str + strlen(str) - 1;
	while(end > str && isspace(*end))
	{
		--end;
	}

	*(end + 1) = '\0';
	*len = end - str;
	return str;
}

static volatile int _rs;
static int _fd;
static volatile char *_rb[32 * 1024];

void *thread_recv(void *arg)
{
	int n;
	for(;;)
	{
		while(_rs) ;
		if((n = read(_fd, _rb, sizeof(_rb))) < 0)
		{
			_rs = 2;
			break;
		}


		if(n > 0)
		{
			_rs = 1;
		}
	}

	return NULL;
	(void)arg;
}

int main(int argc, char **argv)
{
	int echo = 0, nl = 1;
	WINDOW *win_out;
	char *buf;
	pthread_t p;
	FIELD *field_input[2];
	FORM *form_input;
	int i, ch;
	if(argc < 3)
	{
		fputs("Usage: ./terminal port baudrate [echo] [nonl|crlf]\n", stderr);
		return 1;
	}

	if(argc == 4)
	{
		if(strcmp(argv[3], "echo") == 0)
		{
			echo = 1;
		}
		else if(strcmp(argv[3], "nonl") == 0)
		{
			nl = 0;
		}
		else if(strcmp(argv[3], "crlf") == 0)
		{
			nl = 2;
		}
		else
		{
			fputs("Invalid Argument\n", stderr);
			return 1;
		}
	}
	else if(argc == 5)
	{
		if(strcmp(argv[3], "echo") == 0)
		{
			echo = 1;
			if(strcmp(argv[3], "nonl") == 0)
			{
				nl = 0;
			}
			else if(strcmp(argv[4], "crlf") == 0)
			{
				nl = 2;
			}
			else
			{
				fputs("Invalid Argument\n", stderr);
				return 1;
			}
		}
		else if(strcmp(argv[3], "nonl") == 0)
		{
			if(strcmp(argv[4], "echo") == 0)
			{
				echo = 1;
			}
			else
			{
				fputs("Invalid Argument\n", stderr);
				return 1;
			}
		}
		else if(strcmp(argv[3], "crlf") == 0)
		{
			if(strcmp(argv[4], "echo") == 0)
			{
				echo = 1;
			}
			else
			{
				fputs("Invalid Argument\n", stderr);
				return 1;
			}
		}
		else
		{
			fputs("Invalid Argument\n", stderr);
			return 1;
		}
	}
	else if(argc > 5)
	{
		fputs("Invalid Argument\n", stderr);
		return 1;
	}

	if((_fd = init_fd(argv[1], atoi(argv[2]))) < 0)
	{
		fprintf(stderr, "Failed to open port \"%s\"\n", argv[1]);
		return 1;
	}

	if(pthread_create(&p, NULL, thread_recv, NULL))
	{
		return 1;
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	win_out = newwin(LINES - 2, COLS, 0, 0);
	scrollok(win_out, TRUE);

	field_input[0] = new_field(1, COLS - 2, LINES - 1, 2, 0, 0);
	field_opts_off(field_input[0], O_AUTOSKIP);
	field_input[1] = NULL;
	form_input = new_form(&field_input[0]);
	post_form(form_input);
	refresh();
	for(i = 0; i < COLS; ++i)
	{
		mvprintw(LINES - 2, i, "-");
	}

	mvprintw(LINES - 1, 0, "] ");
	refresh();
	for(;;)
	{
		if(_rs == 1)
		{
			wprintw(win_out, "%s", _rb);
			wrefresh(win_out);
			_rs = 0;
		}
		else if(_rs == 2)
		{
			goto leave;
		}

		ch = kbhit();
		switch(ch)
		{
			case KEY_F(9):
				goto leave;

			case KEY_LEFT:
				form_driver(form_input, REQ_PREV_CHAR);
				break;

			case KEY_RIGHT:
				form_driver(form_input, REQ_NEXT_CHAR);
				break;

			case 127:
				form_driver(form_input, REQ_PREV_CHAR);
				form_driver(form_input, REQ_DEL_CHAR);
				break;

			case KEY_DC:
				form_driver(form_input, REQ_DEL_CHAR);
				break;

			case KEY_HOME:
				form_driver(form_input, REQ_BEG_FIELD);
				break;

			case KEY_END:
				form_driver(form_input, REQ_END_FIELD);
				break;

			case 10:
			{
				int len;
				char *str;
				form_driver(form_input, REQ_VALIDATION);
				buf = field_buffer(field_input[0], 0);
				str = _trim(buf, &len);
				if(len == 0)
				{
					break;
				}

				if(echo)
				{
					wprintw(win_out, "%s\n", buf);
					wrefresh(win_out);
				}

				if(nl == 1)
				{
					str[++len] = '\n';
					++len;
				}
				else if(nl == 2)
				{
					str[++len] = '\r';
					str[++len] = '\n';
					++len;
				}

				write(_fd, str, len);
				form_driver(form_input, REQ_CLR_FIELD);
				break;
			}

			default:
				if(ch >= 32 && ch <= 126)
				{
					form_driver(form_input, ch);
				}
				break;
		}
	}

leave:
	unpost_form(form_input);
	free_form(form_input);
	free_field(field_input[0]);
	endwin();
	return 0;
}