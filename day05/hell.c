#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define BUFSIZE 4096

int	main(int ac, char **av)
{
	ssize_t ret;
	int	fd = open("/dev/fortytwo", O_RDWR);
	int	devurand = open("/dev/urandom", O_RDONLY);
	size_t	idx;
	char	buf[BUFSIZE];

	dprintf(STDOUT_FILENO, "TESTING open device forty two ");
	assert(fd > -1);
	dprintf(STDOUT_FILENO, "OK\n");

	dprintf(STDOUT_FILENO, "TESTING read 1 by 1 ");
	for (int i = 0; i < LOGIN_LEN; i++) {
		ret = read(fd, buf + i, 1);
		assert(ret > 0);
	}
	assert(memcmp(buf, LOGIN, LOGIN_LEN) == 0);
	dprintf(STDOUT_FILENO, "OK\n");

	dprintf(STDOUT_FILENO, "TESTING end of buffer ");
	ret = read(fd, buf, 1);
	assert(ret == 0);
	dprintf(STDOUT_FILENO, "OK\n");

	dprintf(STDOUT_FILENO, "TESTING random buffer write ");
	for (int i = 0; i < 128; i++) {
		uint8_t _size;
		int fail;

		read(devurand, &_size, sizeof(_size));
		read(devurand, buf, _size);

		fail = _size % 2;
		if (fail) {
			ret = write(fd, buf, _size);
			assert(ret < 0);
			dprintf(STDOUT_FILENO, "test %d expect fail %s\n", i,
				errno != EINVAL ?
				"WARN, return is not EINVAL" :
				"OK");
		} else {
			ret = write(fd, LOGIN, LOGIN_LEN);
			assert(ret >= 0);
			dprintf(STDOUT_FILENO, "test %d expect success OK\n", i);
		}
	}

	dprintf(STDOUT_FILENO, "TESTING write 1 by 1 ");
	for (int i = 0; i < LOGIN_LEN; i++) {
		ret = write(fd, &LOGIN[i], 1);
		assert(ret >= 0);
	}
	dprintf(STDOUT_FILENO, "OK\n");

	dprintf(STDOUT_FILENO, "TESTING random buffer write 1 by 1\n");
	for (int i = 0; i < 128; i++) {
		uint8_t _size;
		int fail;

		read(devurand, &_size, sizeof(_size));
		_size &= 7;
		read(devurand, buf, _size);

		fail = _size % 2;
		if (fail) {
			int __expected;
			int z = 0;
			dprintf(STDOUT_FILENO, "test %d expect fail ", i);
			for (int i = 0; i < _size; i++) {
				ret = write(fd, buf + i, 1);
				__expected = LOGIN[z] == buf[i];
				z += __expected ? 1 : -z;
				assert(__expected || ret < 0);
			}
			assert(__expected || ret < 0);
			dprintf(STDOUT_FILENO, errno != EINVAL ?
				"WARN, return is not EINVAL\n" :
				"OK\n");
		} else {
			/* einval has weird effect, trying to reset position does not work */
			for (int i = 0; i < LOGIN_LEN; i++) {
				write(fd, LOGIN + i, 1);	
			}

			dprintf(STDOUT_FILENO, "test %d expect success", i);
			for (int i = 0; i < LOGIN_LEN; i++) {
				ret = write(fd, LOGIN + i, 1);
				assert(ret >= 0);
			}
			assert(ret >= 0);
			dprintf(STDOUT_FILENO, " OK\n");
		}
	}

	return 0;
}
