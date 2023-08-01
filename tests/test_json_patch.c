#ifdef NDEBUG
#undef NDEBUG
#endif
#include "strerror_override.h"
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "json.h"
#include "snprintf_compat.h"

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

void test_json_patch_op(struct json_object *jo)
{
	const char *comment = json_object_get_string(json_object_object_get(jo, "comment"));
	struct json_object *doc = json_object_object_get(jo, "doc");
	struct json_object *patch = json_object_object_get(jo, "patch");
	struct json_object *expected = NULL;
	json_bool have_expected = json_object_object_get_ex(jo, "expected", &expected);
	struct json_object *error = json_object_object_get(jo, "error");
	const char *error_s = json_object_get_string(error);
	struct json_object *res = NULL;
	int ret;

	printf("Testing '%s', doc '%s' patch '%s' : ",
		comment ? comment : error_s,
		json_object_get_string(doc),
		json_object_get_string(patch));
	if (!error && !have_expected) {
		printf("BAD TEST - no expected or error conditions in test: %s\n", json_object_to_json_string(jo));
		assert(0);
	}
	fflush(stdout);
	struct json_patch_error jperr;
	if (error) {
		assert(-1 == json_patch_apply(doc, patch, &res, &jperr));
		assert(jperr.errno_code != 0);
		printf("OK\n");
		printf(" => json_patch_apply failed as expected: %s at patch idx %zu: %s\n",
			strerror(jperr.errno_code), jperr.patch_failure_idx, jperr.errmsg);
		json_object_put(res);
	} else {
		ret = json_patch_apply(doc, patch, &res, &jperr);
		if (ret) {
			fprintf(stderr, "json_patch_apply() returned '%d'\n", ret);
			fprintf(stderr, "Expected: %s\n", json_object_get_string(expected));
			fprintf(stderr, "Got: %s\n", res ? json_object_get_string(res) : "(null)");
			fprintf(stderr, "json_patch_apply failed: %s at patch idx %zu: %s\n",
				strerror(jperr.errno_code), jperr.patch_failure_idx, jperr.errmsg);
			fflush(stderr);
			assert(0);
		}
		// Note: res might be NULL if the whole document was removed
		assert(jperr.errno_code == 0);
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
		printf("OK\n");
	}

}

void test_json_patch_using_file(const char *testdir, const char *filename)
{
	char full_filename[PATH_MAX];
	(void)snprintf(full_filename, sizeof(full_filename), "%s/%s", testdir, filename);
	size_t ii;

	printf("Testing using file %s\n", filename);
	json_object *jo = json_object_from_file(full_filename);
	if (!jo) {
		fprintf(stderr, "FAIL: unable to open %s: %s\n", full_filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (ii = 0; ii < json_object_array_length(jo); ii++) {
		struct json_object *jo1 = json_object_array_get_idx(jo, ii);
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
