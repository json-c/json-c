/*
 * Tests if binary strings are supported.
 */

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_object.h"
#include "json_object_private.h"

int main(void)
{
	/* this test has a space after the null character. check that it's still included */
	char *str = strdup("This string should be longer than 32 characters");
	struct json_object *jso_str = NULL;
	int retval = 0;

	if (!str)
	{
		puts("FAIL: strdup() returned NULL");
		retval = 1;
		goto exit;
	}

	jso_str = json_object_new_string_noalloc(str);

	if (!jso_str)
	{
		puts("FAIL: json_object_new_string_noalloc(str) returned NULL");
		retval = 2;
		goto exit;
	}
	if (str != json_object_get_string(jso_str))
	{
		puts("FAIL: json_object_new_string_noalloc(str) did not own the buffer.");
		retval = 3;
		goto exit;
	}

	json_object_put(jso_str);

	str = strdup("this string is short");
	if (!str)
	{
		puts("FAIL: strdup() returned NULL");
		retval = 4;
		goto exit;
	}

	jso_str = json_object_new_string_noalloc(str);

	if (!jso_str)
	{
		puts("FAIL: json_object_new_string_noalloc(str) returned NULL");
		retval = 5;
		goto exit;
	}
	if (str == json_object_get_string(jso_str))
	{
		puts("FAIL: json_object_new_string_noalloc(str) owns the pointer instead using the internal buffer.");
		retval = 6;
		goto exit;
	}
	if (json_object_get_string(jso_str) != jso_str->o.c_string.str.data)
	{
		puts("FAIL: json_object_new_string_noalloc(str) does not use the internal buffer.");
		free(str);
		retval = 7;
		goto exit;
	}

	puts("PASS");

exit:
	if (jso_str)
		json_object_put(jso_str);
	return retval;
}
