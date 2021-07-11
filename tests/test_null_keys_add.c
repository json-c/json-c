/*
 * Tests if binary strings are supported.
 */

#include "config.h"
#include <stdio.h>
#include <string.h>

#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"
#include "linkhash.h"

int main(void)
{
	/* this test has embedded null characters in the key and value.
	 * check that it's still included after deserializing. */
	const char *input = "{ \"foo\": 14.5, \"bar\": [] }";
	const char *foo_key = "foo";
	const char *foo_value = "14.5";
	const char *bar_key = "bar";

	const char *toadd_key = "foo\0bar";
	const int toadd_key_len = 7;
	const char *toadd_key_printable = "foo\\0bar";
	const char *toadd_value = "qwerty\0asdf";
	const int toadd_value_len = 12;
	const char *toadd_value_printable = "qwerty\\0asdf";

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
	if (json_object_object_length(parsed) != 2)
	{
		printf("Should contain two fields (has %d) (error!)",
		       json_object_object_length(parsed));
		return 1;
	}
	json_bool key_present = json_object_object_get_ex(parsed, foo_key, NULL);
	if (!key_present)
	{
		printf("Should contain key \"%s\", but does not (error!)", foo_key);
		return 1;
	}
	key_present = json_object_object_get_ex(parsed, bar_key, NULL);
	if (!key_present)
	{
		printf("Should contain key \"%s\", but does not (error!)", bar_key);
		return 1;
	}

	// Check the previous keys are still the same present
	struct json_object *foo = json_object_object_get(parsed, foo_key);
	if (!json_object_is_type(foo, json_type_double))
	{
		printf("Key \"%s\" should be `json_type_double` (%d) but was %d (error!)\n",
		       foo_key, (int)json_type_double, json_object_get_type(foo));
		return 1;
	}
	else
	{
		printf("Key \"%s\" parsed as right type\n", foo_key);
	}
	struct json_object *bar = json_object_object_get(parsed, bar_key);
	if (!json_object_is_type(bar, json_type_array))
	{
		printf("Key \"%s\" should be `json_type_array` (%d) but was %d (error!)\n", bar_key,
		       (int)json_type_array, json_object_get_type(bar));
		return 1;
	}
	else
	{
		printf("Key \"%s\" parsed as right type\n", bar_key);
	}

	// Add the new key
	struct json_object *new_str = json_object_new_string_len(toadd_value, toadd_value_len);
	if (json_object_object_add_ex_len(parsed, toadd_key, toadd_key_len, new_str,
	                                  JSON_C_OBJECT_KEY_IS_CONSTANT) != 0)
	{
		printf("An error occured adding the key \"%s\" (error!)\n", toadd_key_printable);
		return 1;
	}

	// Check the new key was actually added
	if (json_object_object_length(parsed) != 3)
	{
		printf("Should contain three fields after adding new key (has %d) (error!)",
		       json_object_object_length(parsed));
		return 1;
	}
	else if (json_object_object_get_len(parsed, toadd_key, toadd_key_len) != new_str)
	{
		printf("Have three keys, but don't have the right value in \"%s\" (error!)\n",
		       toadd_key_printable);
		printf("Keys :\n");
		json_object_object_foreach(parsed, key, val)
		{
			putchar('\"');
			fwrite(json_key_data(key), json_key_size(key), 1, stdout);
			printf("\" (%zd)\n", json_key_size(key));
		}
		return 1;
	}
	else
	{
		printf("Added the key \"%s\" successfully\n", toadd_key_printable);
	}

	// Check the previous keys are still the same present
	foo = json_object_object_get(parsed, foo_key);
	if (!json_object_is_type(foo, json_type_double))
	{
		printf("Key \"%s\" should be `json_type_double` (%d) but was %d (error!)\n",
		       foo_key, (int)json_type_double, json_object_get_type(foo));
		return 1;
	}
	else
	{
		printf("Key \"%s\" is still the same\n", foo_key);
	}
	bar = json_object_object_get(parsed, bar_key);
	if (!json_object_is_type(bar, json_type_array))
	{
		printf("Key \"%s\" should be `json_type_array` (%d) but was %d (error!)\n", bar_key,
		       (int)json_type_array, json_object_get_type(bar));
		return 1;
	}
	else
	{
		printf("Key \"%s\" is still the same\n", bar_key);
	}

	json_object_put(parsed);
	printf("PASS\n");
	return 0;
}
