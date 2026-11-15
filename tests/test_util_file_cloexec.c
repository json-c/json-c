#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "json.h"

struct exec_check
{
	const char *self;
	const char *fifo;
	const char *ready;
	int release;
};

static void wait_child(pid_t pid)
{
	int status;
	assert(waitpid(pid, &status, 0) == pid);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

static void *check_exec(void *arg)
{
	struct exec_check *check = arg;
	struct stat fifo_stat, fd_stat;
	struct timespec delay = {0, 1000000};
	long max_fd = sysconf(_SC_OPEN_MAX);
	int fd, found = -1;
	char fd_arg[32];
	pid_t pid;
	int status;
	while (access(check->ready, F_OK) != 0)
	{
		assert(errno == ENOENT);
		nanosleep(&delay, NULL);
	}
	assert(stat(check->fifo, &fifo_stat) == 0);
	assert(max_fd > 0);
	while (found < 0)
	{
		for (fd = 0; fd < max_fd; fd++)
		{
			if (fstat(fd, &fd_stat) == 0 && fd_stat.st_dev == fifo_stat.st_dev &&
			    fd_stat.st_ino == fifo_stat.st_ino)
			{
				assert(found == -1);
				found = fd;
			}
		}
		if (found < 0)
			nanosleep(&delay, NULL);
	}
	snprintf(fd_arg, sizeof(fd_arg), "%d", found);
	pid = fork();
	assert(pid >= 0);
	if (pid == 0)
	{
		execl(check->self, check->self, "--check-fd", fd_arg, (char *)NULL);
		_exit(127);
	}
	assert(waitpid(pid, &status, 0) == pid);
	assert(write(check->release, "x", 1) == 1);
	assert(close(check->release) == 0);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	return NULL;
}

static void write_all(int fd, const char *data, size_t len)
{
	while (len > 0)
	{
		ssize_t written = write(fd, data, len);
		assert(written > 0);
		data += written;
		len -= written;
	}
}

static void check_fifo(const char *self, const char *fifo, const char *ready,
                       json_object *obj, int reading)
{
	const char *document = json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
	size_t document_len = strlen(document);
	int release[2];
	pid_t helper;
	pthread_t thread;
	struct exec_check check;
	assert(pipe(release) == 0);
	assert(fcntl(release[1], F_SETFD, FD_CLOEXEC) == 0);
	helper = fork();
	assert(helper >= 0);
	if (helper == 0)
	{
		char buf[4096];
		ssize_t count;
		size_t total = 1;
		int flag, fd;
		assert(close(release[1]) == 0);
		fd = open(fifo, reading ? O_WRONLY : O_RDONLY);
		assert(fd >= 0);
		if (reading)
			write_all(fd, document, document_len / 2);
		else
			assert(read(fd, buf, 1) == 1);
		flag = open(ready, O_WRONLY | O_CREAT | O_EXCL, 0600);
		assert(flag >= 0);
		assert(close(flag) == 0);
		assert(read(release[0], buf, 1) == 1);
		assert(close(release[0]) == 0);
		if (reading)
			write_all(fd, document + document_len / 2, document_len - document_len / 2);
		else
		{
			while ((count = read(fd, buf, sizeof(buf))) > 0)
				total += count;
			assert(count == 0 && total == document_len);
		}
		assert(close(fd) == 0);
		_exit(0);
	}
	assert(close(release[0]) == 0);
	check.self = self;
	check.fifo = fifo;
	check.ready = ready;
	check.release = release[1];
	assert(pthread_create(&thread, NULL, check_exec, &check) == 0);
	if (reading)
	{
		json_object *read_obj = json_object_from_file(fifo);
		assert(read_obj != NULL);
		assert(json_object_equal(obj, read_obj));
		json_object_put(read_obj);
	}
	else
		assert(json_object_to_file(fifo, obj) == 0);
	assert(pthread_join(thread, NULL) == 0);
	wait_child(helper);
	assert(unlink(ready) == 0);
}

int main(int argc, char **argv)
{
	char directory[] = "cloexec-XXXXXX";
	char fifo[64], ready[64], path[64];
	size_t payload_len = 1024 * 1024;
	char *payload;
	json_object *obj;
	int fd;
	if (argc == 3 && strcmp(argv[1], "--check-fd") == 0)
	{
		errno = 0;
		return fcntl(atoi(argv[2]), F_GETFD) == -1 && errno == EBADF ? 0 : 1;
	}
	alarm(20);
	assert(mkdtemp(directory) != NULL);
	snprintf(fifo, sizeof(fifo), "%s/fifo", directory);
	snprintf(ready, sizeof(ready), "%s/ready", directory);
	snprintf(path, sizeof(path), "%s/file.json", directory);
	assert(mkfifo(fifo, 0600) == 0);
	payload = malloc(payload_len);
	assert(payload != NULL);
	memset(payload, 'x', payload_len);
	obj = json_object_new_string_len(payload, payload_len);
	free(payload);
	assert(obj != NULL);
	check_fifo(argv[0], fifo, ready, obj, 1);
	check_fifo(argv[0], fifo, ready, obj, 0);

	fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0600);
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
	assert(unlink(fifo) == 0);
	assert(rmdir(directory) == 0);
	alarm(0);
	puts("close-on-exec file descriptors OK");
	return 0;
}
