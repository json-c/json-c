#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "json.h"

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

void test_json_patch_op(struct json_object *jo)
{
	const char *comment = json_object_get_string(json_object_object_get(jo, "comment"));
	struct json_object *doc = json_object_object_get(jo, "doc");
	struct json_object *patch = json_object_object_get(jo, "patch");
	struct json_object *expected = json_object_object_get(jo, "expected");
	struct json_object *error = json_object_object_get(jo, "error");
	int disabled_test = json_object_get_boolean(json_object_object_get(jo, "disabled_in_json_c"));
	const char *error_s = json_object_get_string(error);
	struct json_object *res = NULL;
	int ret;

	printf("Testing '%s', doc '%s' patch '%s' : ",
		comment ? comment : error_s,
		json_object_get_string(doc),
		json_object_get_string(patch));
	if (disabled_test) {
		printf("SKIPPING - disabled in the test spec\n");
		return;
	}
	if (!error && !expected) {
		printf("SKIPPING - no expected or error conditions in test\n");
		return;
	}
	fflush(stdout);
	if (error) {
		assert(-1 == json_patch_apply(doc, patch, &res));
		assert(res == NULL);
	} else {
		ret = json_patch_apply(doc, patch, &res);
		if (ret) {
			fprintf(stderr, "json_patch_apply() returned '%d'\n", ret);
			fprintf(stderr, "Expected: %s\n", json_object_get_string(expected));
			fprintf(stderr, "Got: %s\n", json_object_get_string(res));
			fflush(stderr);
			assert(0);
		}
		assert(res != NULL);
		ret = json_object_equal(expected, res);
		if (ret == 0) {
			fprintf(stderr, "json_object_equal() returned '%d'\n", ret);
			fprintf(stderr, "Expected: %s\n", json_object_get_string(expected));
			fprintf(stderr, "Got: %s\n", json_object_get_string(res));
			fflush(stderr);
			assert(0);
		}
		json_object_put(res);
		res = NULL;
	}

	printf("OK\n");
}

void test_json_patch_using_file(const char *testdir, const char *filename)
{
	char full_filename[PATH_MAX];
	(void)snprintf(full_filename, sizeof(full_filename), "%s/%s", testdir, filename);
	int i;

	json_object *jo = json_object_from_file(full_filename);
	if (!jo) {
		fprintf(stderr, "FAIL: unable to open %s: %s\n", full_filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < json_object_array_length(jo); i++) {
		struct json_object *jo1 = json_object_array_get_idx(jo, i);
		test_json_patch_op(jo1);
	}

	json_object_put(jo);
}

int main(int argc, char **argv)
{
	const char *testdir;
	if (argc < 2)
	{
		fprintf(stderr,
			"Usage: %s <testdir>\n"
			"  <testdir> is the location of input files\n",
			argv[0]);
		return EXIT_FAILURE;
	}
	testdir = argv[1];

	//      Test json_c_version.c
	if (strncmp(json_c_version(), JSON_C_VERSION, sizeof(JSON_C_VERSION)))
	{
		printf("FAIL: Output from json_c_version(): %s does not match %s",
		       json_c_version(), JSON_C_VERSION);
		return EXIT_FAILURE;
	}
	if (json_c_version_num() != JSON_C_VERSION_NUM)
	{
		printf("FAIL: Output from json_c_version_num(): %d does not match %d",
		       json_c_version_num(), JSON_C_VERSION_NUM);
		return EXIT_FAILURE;
	}

	test_json_patch_using_file(testdir, "json_patch_spec_tests.json");
	test_json_patch_using_file(testdir, "json_patch_tests.json");
	return 0;
}
