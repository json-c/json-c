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

	return EXIT_SUCCESS;
}
