#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#define BUFSIZE 4096

int	main(int ac, char **av)
{
	ssize_t ret;
	int	fd = open("/dev/fortytwo", O_RDWR);
	char	buf[BUFSIZE];

	dprintf(STDOUT_FILENO, "TESTING open device forty two");
	assert(fd > -1);
	dprintf(STDOUT_FILENO, "OK\n");
	ret = read(fd, buf, BUFSIZE);
	dprintf(STDOUT_FILENO, "TESTING read device forty two");
	assert(ret > -1);
	dprintf(STDOUT_FILENO, "OK\n");
	buf[ret] = '\n';
	dprintf(STDOUT_FILENO, "TESTING write %s device forty two", LOGIN);
	write(STDIN_FILENO, buf, ret + 1);
	ret = write(fd, LOGIN, LOGIN_LEN);
	assert(ret > -1);
	dprintf(STDOUT_FILENO, "OK\n");
	dprintf(STDOUT_FILENO, "TESTING bad write device forty two", LOGIN);
	ret = write(fd, "asdasd", 6);
	dprintf(STDOUT_FILENO, "OK\n");
	assert(ret <= -1);
	return ret;
}
