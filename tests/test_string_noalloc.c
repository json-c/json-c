/*
 * Tests if binary strings are supported.
 */

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
	char *str1 = strdup("This string should be longer than 32 characters");
	char *str2 = strdup("this string is short");
	struct json_object *jso_str = NULL;

	if (!str1 || !str2)
	{
		puts("FAIL: strdup() returned NULL");
		return 1;
	}

	jso_str = json_object_new_string_noalloc(str1);
	if (!jso_str)
	{
		puts("FAIL: json_object_new_string_noalloc(str1) returned NULL");
		return 2;
	}

	json_object_put(jso_str);
	jso_str = NULL;

	jso_str = json_object_new_string_noalloc(str2);
	if (!jso_str)
	{
		puts("FAIL: json_object_new_string_noalloc(str2) returned NULL");
		return 3;
	}

	json_object_put(jso_str);

	puts("PASS");
	return 0;
}
