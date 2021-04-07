#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

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

static int init_fd(char *port)
{
	int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		printf("(open) Error %d: %s\n", errno, strerror(errno));
		return -1;
	}

	set_interface_attribs(fd, B1200, 0);
	set_blocking(fd, 1);
	return fd;
}
