/*
 * Tests if binary strings are supported.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include "config.h"

#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"

int main(void)
{
	/* this test has a space after the null character. check that it's still included */
	char *str = strdup("This string should be longer than 32 characters");
	struct json_object *jso_str = NULL;

	if (!str)
	{
		puts("FAIL: strdup() returned NULL");
		return 1;
	}

	jso_str = json_object_new_string_noalloc(str);
	if (!jso_str)
	{
		puts("FAIL: json_object_new_string_noalloc(str) returned NULL");
		free(str);
		return 2;
	}

	json_object_put(jso_str);
	jso_str = NULL;

	str = strdup("this string is short");
	if (!str)
	{
		puts("FAIL: strdup() returned NULL");
		return 1;
	}

	jso_str = json_object_new_string_noalloc(str);
	if (!jso_str)
	{
		puts("FAIL: json_object_new_string_noalloc(str) returned NULL");
		free(str);
		return 3;
	}

	json_object_put(jso_str);

	puts("PASS");
	return 0;
}
