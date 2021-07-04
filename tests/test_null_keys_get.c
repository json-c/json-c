/*
 * Tests if binary strings are supported.
 */

#include "config.h"
#include <stdio.h>
#include <string.h>

#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"

int main(void)
{
	/* this test has embedded null characters in the key and value.
	 * check that it's still included after deserializing. */
	const char *input = "{ \"foo\\u0000bar\": \"qwerty\\u0000asdf\" }";
	const char *expected_key = "foo\0bar";
	const int expected_key_len = 7;
	const char *expected_key_printable = "foo\\0bar";
	const char *expected_value = "qwerty\0asdf";
	const int expected_value_len = 12;
	const char *expected_value_printable = "qwerty\\0asdf";

	struct json_object *parsed = json_tokener_parse(input);

	printf("Parsed input: %s\n", input);
	printf("Result is ");
	if (parsed == NULL)
	{
		printf("NULL (error!)\n");
		return 1; // Error somewhere
	}
	else if (!json_object_is_type(parsed, json_type_object))
	{
		printf("not `json_type_object` (error!)\n");
		return 1; // Error somewhere
	}
	else
	{
		printf("`json_type_object`\n");
	}

	// Check nothing odd happened in parsing
	if (json_object_object_length(parsed) != 1)
	{
		printf("Should contain only one field (has %d) (error!)",
		       json_object_object_length(parsed));
		return 1;
	}
	json_bool key_present = json_object_object_get_ex(parsed, expected_key, NULL);
	if (key_present)
	{
		printf("The key \"%s\" should not be present when calling "
		       "`json_object_object_get_ex` "
		       "(the real key contains a NUL character). (error!)\n",
		       expected_key);
		return 1;
	}

	// Check the key is present
	struct json_object *string;
	key_present = json_object_object_get_ex_len(parsed, expected_key, 7, &string);
	if (!key_present)
	{
		printf("The key \"%s\" should be present when calling "
		       "`json_object_object_get_ex_len` (error!)\n",
		       expected_key_printable);
		return 1;
	}
	else if (string == NULL)
	{
		printf("The key \"%s\" was present when calling "
		       "`json_object_object_get_ex_len`, but got NULL (error!)\n",
		       expected_key_printable);
		return 1;
	}
	struct json_object *from_get_len = json_object_object_get_len(parsed, expected_key, 7);
	if (string != from_get_len)
	{
		printf("The value returned from `json_object_object_get_len` should be the "
		       "same as the one from `json_object_object_get_ex_len` (error!)\n"
		       "Got %p from `json_object_object_get_ex_len` but %p from "
		       "`json_object_object_get_len`\n",
		       string, from_get_len);
		return 1;
	}
	else
	{
		printf("Key was present and same for "
		       "`json_object_object_get_ex_len` and `json_object_object_get_len`\n");
	}

	// Check the value is right
	if (!json_object_is_type(string, json_type_string))
	{
		printf(
		    "Value is wrong type, expected `json_type_string` (%d) but got %d (error!)\n",
		    json_type_string, json_object_get_type(string));
		return 1;
	}
	else
	{
		printf("Value is right type, `json_type_string`\n");
	}
	const int actual_value_len = json_object_get_string_len(string);
	const char *actual_value = json_object_get_string(string);
	if (actual_value_len != expected_value_len)
	{
		printf("Value is wrong length, expected %d but got %d (error!)\n",
		       expected_value_len, actual_value_len);
		return 1;
	}
	else if (memcmp(expected_value, actual_value, actual_value_len) != 0)
	{
		printf("Expected \"%s\" but got \"", expected_value_printable);
		for (int i = 0; i < actual_value_len; ++i)
		{
			if (actual_value[i] == '\0')
			{
				puts("\\0");
			}
			else
			{
				putchar(actual_value[i]);
			}
		}
		printf("\"\n");
		return 1;
	}
	else
	{
		printf("Expected value matches actual\n");
	}

	json_object_put(parsed);
	printf("PASS\n");
	return 0;
}
