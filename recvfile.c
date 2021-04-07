
#include <stdio.h>
#include <ctype.h>
#include "xfer.c"

static int recv_file(int fd, char *filename)
{
	int i, n, len, dig, nd;
	char buf[32768], *p = buf;
	FILE *fp;
	if(!(fp = fopen(filename, "w")))
	{
		fprintf(stderr, "Error opening file for writing\n");
		return -1;
	}

	for(;;)
	{
		if((n = read(fd, p, sizeof(buf) - (p - buf))) < 0)
		{
			goto error;
		}

		if(n == 0)
		{
			goto exit;
		}

		for(i = 0; i < n; ++i)
		{
			if(p[i] == 0x1A)
			{
				goto exit;
			}
		}

		p += n;
	}

exit:
	dig = 0;
	len = p - buf;
	for(i = 0; i < len; ++i)
	{
		if(i == 0 || buf[i] == '\n')
		{
			while(buf[i] == '\r' || buf[i] == '\n')
			{
				++i;
			}

			for(nd = 0; isdigit(buf[i]); ++i, ++nd);
			if(nd > dig)
			{
				dig = nd;
			}
		}
	}

	++dig;

	/* Write file, remove line numbers, convert to LF and replace spaces with tabs */
	for(i = 0; i < len; ++i)
	{
		for(nd = 0; nd < dig; ++nd, ++i);
		p = buf + i;
		for(nd = 0; buf[i] == ' '; ++nd, ++i)
		{
			buf[i] = '\t';
		}

		for(; buf[i] != '\r' && buf[i] != '\n'; ++nd, ++i);
		while(buf[i] == '\r' || buf[i] == '\n')
		{
			++i;
		}

		--i;
		p[nd++] = '\n';
		if((int)fwrite(p, 1, nd, fp) != nd)
		{
			goto error;
		}
	}

	fclose(fp);
	return 0;

error:
	fclose(fp);
	return 1;
}

int main(int argc, char **argv)
{
	int fd;
	if(argc != 3)
	{
		fputs(
			"SHARP PC-G850V RecvFile Utility\n"
			"Usage: ./recvfile port filename\n", stderr);
		return 1;
	}

	if((fd = init_fd(argv[1])) < 0)
	{
		return 1;
	}

	if(recv_file(fd, argv[2]) < 0)
	{
		fputs("Failed to recieve file\n", stderr);
		close(fd);
		return 1;
	}

	close(fd);
	return 0;
}
