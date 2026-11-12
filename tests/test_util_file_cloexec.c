#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "json.h"

static const char *self;
static int check_open;
static int checked_opens;

/* Interpose open so the exec happens while json-c still owns its descriptor. */
int open(const char *path, int flags, ...)
{
	int mode = 0;
	int fd;
	if (flags & O_CREAT)
	{
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, int);
		va_end(ap);
	}
	fd = openat(AT_FDCWD, path, flags, mode);
	if (fd >= 0 && check_open)
	{
		char fd_arg[32];
		pid_t pid;
		int status;
		checked_opens++;
		snprintf(fd_arg, sizeof(fd_arg), "%d", fd);
		pid = fork();
		assert(pid >= 0);
		if (pid == 0)
		{
			execl(self, self, "--check-fd", fd_arg, (char *)NULL);
			_exit(127);
		}
		assert(waitpid(pid, &status, 0) == pid);
		assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	}
	return fd;
}

int main(int argc, char **argv)
{
	const char *path = "cloexec.json";
	json_object *obj;
	int fd;
	if (argc == 3 && strcmp(argv[1], "--check-fd") == 0)
	{
		errno = 0;
		return fcntl(atoi(argv[2]), F_GETFD) == -1 && errno == EBADF ? 0 : 1;
	}
	self = argv[0];
	obj = json_object_new_object();
	assert(obj != NULL);
	check_open = 1;
	assert(json_object_to_file(path, obj) == 0);
	json_object_put(obj);
	obj = json_object_from_file(path);
	assert(obj != NULL);
	assert(checked_opens == 2);
	check_open = 0;

	/* Caller-owned descriptors must keep their existing inheritance policy. */
	fd = open(path, O_RDWR);
	assert(fd >= 0);
	assert(fcntl(fd, F_GETFD) == 0);
	assert(json_object_to_fd(fd, obj, JSON_C_TO_STRING_PLAIN) == 0);
	assert(fcntl(fd, F_GETFD) == 0);
	assert(lseek(fd, 0, SEEK_SET) == 0);
	json_object_put(obj);
	obj = json_object_from_fd(fd);
	assert(obj != NULL);
	assert(fcntl(fd, F_GETFD) == 0);
	json_object_put(obj);
	assert(close(fd) == 0);
	assert(unlink(path) == 0);
	puts("close-on-exec file descriptors OK");
	return 0;
}
