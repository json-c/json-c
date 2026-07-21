#ifdef NDEBUG
#undef NDEBUG
#endif
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include "config.h"

#include "json.h"

#define NESTING_DEPTH 100000

static char *generate_json_string(void)
{
	char *str;
	int depth = NESTING_DEPTH;
	str = malloc(depth * 2 + 1);
	memset(str, '[', depth);
	memset(str + depth, ']', depth);
	str[depth * 2] = '\0';
	return str;
}

static void test_deep_nesting_put(const char *str)
{
	json_object *my_array;

	struct json_tokener *tok = json_tokener_new_ex(NESTING_DEPTH);
	my_array = json_tokener_parse_ex(tok, str, strlen(str) + 1);
	printf("Parsed depth %d string to json_object: %s\n", NESTING_DEPTH, (my_array == NULL) ? "NO" : "yes");

	json_object_put(my_array);
	printf("Freed json_object\n");

	json_tokener_free(tok);
}

static void test_deep_nesting_tostring(const char *str)
{
	json_object *my_array;

	struct json_tokener *tok = json_tokener_new_ex(NESTING_DEPTH);
	my_array = json_tokener_parse_ex(tok, str, strlen(str) + 1);
	printf("Parsed depth %d string to json_object: %s\n", NESTING_DEPTH, (my_array == NULL) ? "NO" : "yes");

	const char *res = json_object_to_json_string_ext(my_array, JSON_C_TO_STRING_PLAIN);
	printf("Serialized to string of length %ld\n", (long)strlen(res));
	json_object_put(my_array);
	printf("Freed json_object\n");

	json_tokener_free(tok);
}

struct userdata_test {
	int userdata_val;
	char *p;
};
/*
 * Check that the user_delete function is only called once, even with the
 * newer code to avoid deeply nested calls during json_object_put().
 */
static void user_delete_test(struct json_object *jso, void *userdata_in)
{
	struct userdata_test *userdata = (struct userdata_test *)userdata_in;
	printf("in user_delete, userdata_val=%d\n", userdata->userdata_val);
	fflush(stdout);
	userdata->userdata_val = 0;
	userdata->p[0] = 'x';
	userdata->p[8191] = 'x';
	free(userdata->p);
}
static void test_nesting_with_user_delete(void)
{
	json_object *jso;
	struct userdata_test userdata_val = {
		1, malloc(8192)
	};

	jso = json_object_new_object();
	json_object_set_userdata(jso, &userdata_val, user_delete_test);
	json_object_object_add(jso, "somekey", json_object_new_string("foo"));
	json_object_put(jso);
}

/*
 * Check that json_object_put() returns 1 whenever the object is freed,
 * including for scalars and empty containers, and 0 only when a
 * reference remains.
 */
static void test_put_return_values(void)
{
	json_object *jso;

	printf("put(empty object) returned %d\n", json_object_put(json_object_new_object()));
	printf("put(empty array) returned %d\n", json_object_put(json_object_new_array()));
	printf("put(string) returned %d\n", json_object_put(json_object_new_string("foo")));
	printf("put(int) returned %d\n", json_object_put(json_object_new_int(42)));

	jso = json_object_new_object();
	json_object_object_add(jso, "somekey", json_object_new_string("foo"));
	printf("put(non-empty object) returned %d\n", json_object_put(jso));

	jso = json_object_new_object();
	json_object_get(jso);
	printf("put(still-referenced object) returned %d\n", json_object_put(jso));
	printf("put(last reference) returned %d\n", json_object_put(jso));
}

int main(int argc, char **argv)
{
	char *str;	
#ifdef HAVE_SETRLIMIT
	struct rlimit rl;
	rl.rlim_cur = 2048;
	rl.rlim_max = 2048;
	setrlimit(RLIMIT_STACK, &rl);
#endif

	str = generate_json_string();

	MC_SET_DEBUG(1);

	test_deep_nesting_put(str);

	if (0) // TODO: make json_object_to_json_string non-recursive
		test_deep_nesting_tostring(str);

	free(str);

	test_nesting_with_user_delete();

	test_put_return_values();

	return EXIT_SUCCESS;
}
