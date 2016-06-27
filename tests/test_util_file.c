#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "json.h"
#include "json_util.h"

static void test_read_valid_with_fd(const char *testdir);
static void test_read_nonexistant();
static void test_read_closed(void);

static void test_write_to_file();
static void stat_and_cat(const char *file);

static void test_write_to_file()
{
	json_object *jso;

	jso = json_tokener_parse("{"
		"\"foo\":1234,"
		"\"foo1\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo2\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo3\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo4\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo5\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo6\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo7\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo8\":\"abcdefghijklmnopqrstuvwxyz\","
		"\"foo9\":\"abcdefghijklmnopqrstuvwxyz\""
		"}");
	const char *outfile = "json.out";
	int rv = json_object_to_file(outfile, jso);
	printf("%s: json_object_to_file(%s, jso)=%d\n",
		(rv == 0) ? "OK" : "FAIL", outfile, rv);
	if (rv == 0)
		stat_and_cat(outfile);

	printf("\n");

	const char *outfile2 = "json2.out";
	rv = json_object_to_file_ext(outfile2, jso, JSON_C_TO_STRING_PRETTY);
	printf("%s: json_object_to_file_ext(%s, jso, JSON_C_TO_STRING_PRETTY)=%d\n",
	     (rv == 0) ? "OK" : "FAIL", outfile2, rv);
	if (rv == 0)
		stat_and_cat(outfile2);
}

static void stat_and_cat(const char *file)
{
	struct stat sb;
	int d = open(file, O_RDONLY, 0600);
	if (d < 0)
	{
		printf("FAIL: unable to open %s: %s\n", file, strerror(errno));
		return;
	}
	if (fstat(d, &sb) < 0)
	{
		printf("FAIL: unable to stat %s: %s\n", file, strerror(errno));
		close(d);
		return;
	}
	char *buf = malloc(sb.st_size + 1);
	if (read(d, buf, sb.st_size) < sb.st_size)
	{
		printf("FAIL: unable to read all of %s: %s\n", file, strerror(errno));
		close(d);
		return;
	}
	buf[sb.st_size] = '\0';
	printf("file[%s], size=%d, contents=%s\n", file, (int)sb.st_size, buf);
}

int main(int argc, char **argv)
{
//	json_object_to_file(file, obj);
//	json_object_to_file_ext(file, obj, flags);

	const char *testdir;
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <testdir>\n  <testdir> is the location of input files\n", argv[0]);
		exit(1);
	}
	testdir = argv[1];

	test_read_valid_with_fd(testdir);
	test_read_nonexistant();
	test_read_closed();
	test_write_to_file();
}

static void test_read_valid_with_fd(const char *testdir)
{
	const char *filename = "./valid.json";

	int d = open(filename, O_RDONLY, 0);
	if (d < 0)
	{
		fprintf(stderr, "FAIL: unable to open %s: %s\n", filename, strerror(errno));
		exit(1);
	}
	json_object *jso = json_object_from_fd(d);
	if (jso != NULL)
	{
		printf("OK: json_object_from_fd(%s)=%s\n", filename, json_object_to_json_string(jso));
		json_object_put(jso);
	}
	else
	{
		fprintf(stderr, "FAIL: unable to parse contents of %s: %s\n", filename, json_util_get_last_err());
	}
	close(d);
}

static void test_read_nonexistant()
{
	const char *filename = "./not_present.json";

	json_object *jso = json_object_from_file(filename);
	if (jso != NULL)
	{
		printf("FAIL: json_object_from_file(%s) returned %p when NULL expected\n", filename, jso);
		json_object_put(jso);
	}
	else
	{
		printf("OK: json_object_from_file(%s) correctly returned NULL: %s\n",
		       filename, json_util_get_last_err());
	}
}

static void test_read_closed()
{
	// Test reading from a closed fd
	int d = open("/dev/null", O_RDONLY, 0);
	close(d);
	json_object *jso = json_object_from_fd(d);
	if (jso != NULL)
	{
		printf("FAIL: read from closed fd returning non-NULL: %p\n", jso);
		fflush(stdout);
		printf("  jso=%s\n", json_object_to_json_string(jso));
		json_object_put(jso);
		return;
	}
	printf("OK: json_object_from_fd(closed_fd), expecting NULL, EBADF, got:%p, %s\n", jso, json_util_get_last_err());
}
