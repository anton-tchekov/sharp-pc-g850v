#include <stdio.h>
#include <ctype.h>
#include "xfer.c"

static int send_file(int fd, char *filename)
{
	int i, len, spc = 2, lineno = 10;
	char lts[8], line[256];
	unsigned char eb[1] = { 0x1A };
	FILE *fp;
	if(!(fp = fopen(filename, "r")))
	{
		fprintf(stderr, "Error opening file for reading\n");
		return -1;
	}

	/* Find number of lines */
	while(fgets(line, sizeof(line), fp))
	{
		len = strlen(line);
		if(len == 0)
		{
			continue;
		}

		lineno += 10;
	}

	if(lineno < 100)
	{
		spc = 3;
	}
	else if(lineno < 1000)
	{
		spc = 4;
	}
	else if(lineno < 10000)
	{
		spc = 5;
	}

	/* Send file */
	lineno = 10;
	rewind(fp);
	while(fgets(line, sizeof(line), fp))
	{
		len = strlen(line);
		if(len == 0)
		{
			continue;
		}

		i = snprintf(lts, sizeof(lts), "%d ", lineno);
		for(; i < spc; ++i)
		{
			lts[i++] = ' ';
		}

		lts[i] = '\0';
		printf("%s", lts);
		write(fd, lts, i);

		/* Replace Tabs */
		for(i = 0; i < len; ++i)
		{
			if(line[i] == '\t')
			{
				line[i] = ' ';
			}
		}

		/* Convert to CRLF */
		--len;
		for(; len >= 0; --len)
		{
			if(!isspace(line[len]))
			{
				break;
			}
		}

		line[++len] = '\r';
		line[++len] = '\n';
		line[++len] = '\0';
		++len;

		printf("%s", line);
		write(fd, line, len);
		lineno += 10;
	}

	write(fd, eb, 1);
	fclose(fp);
	return 0;
}

int main(int argc, char **argv)
{
	int fd;
	if(argc != 3)
	{
		fputs(
			"SHARP PC-G850V SendFile Utility\n"
			"Usage: ./sendfile port filename\n", stderr);
		return 1;
	}

	if((fd = init_fd(argv[1])) < 0)
	{
		return 1;
	}

	if(send_file(fd, argv[2]) < 0)
	{
		fputs("Failed to send file\n", stderr);
		close(fd);
		return 1;
	}

	close(fd);
	return 0;
}
