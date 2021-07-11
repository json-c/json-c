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
	const char *input = "{ \"biff\\u0000\": null, \"\\u0000zxcvbnm\": 178 }";
	const char *expected_biff = "biff\0";
	const int expected_biff_len = 5;
	const char *expected_biff_printable = "biff\\u0000";
	const char *expected_zxcv = "\0zxcvbnm";
	const int expected_zxcv_len = 8;
	const char *expected_zxcv_printable = "\\u0000zxcvbnm";

	struct json_object *parsed = json_tokener_parse(input);
	struct json_object *val;
	json_bool key_present;

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

	key_present = json_object_object_get_ex_len(parsed, expected_biff, expected_biff_len, &val);
	if (!key_present || !json_object_is_type(val, json_type_null))
	{
		printf("The key \"%s\" should be present and should be NULL (error!)\n",
		       expected_biff_printable);
		return 1;
	}
	key_present = json_object_object_get_ex_len(parsed, expected_zxcv, expected_zxcv_len, &val);
	if (!key_present || !json_object_is_type(val, json_type_int) ||
	    json_object_get_int(val) != 178)
	{
		printf("The key \"%s\" should be present and should be 178 (error!)\n",
		       expected_zxcv_printable);
		return 1;
	}
	printf("Expected keys (\"%s\" and \"%s\") present with expected values\n",
	       expected_biff_printable, expected_zxcv_printable);

	// Delete one key
	json_object_object_del_len(parsed, expected_zxcv, expected_zxcv_len);

	// Check it is gone
	if (json_object_object_length(parsed) != 1)
	{
		printf("Should contain only one field (has %d) (error!)",
		       json_object_object_length(parsed));
		return 1;
	}
	key_present = json_object_object_get_ex_len(parsed, expected_biff, expected_biff_len, &val);
	if (!key_present || !json_object_is_type(val, json_type_null))
	{
		printf("The key \"%s\" should be present and should be NULL (error!)\n",
		       expected_biff_printable);
		return 1;
	}
	key_present = json_object_object_get_ex_len(parsed, expected_zxcv, expected_zxcv_len, &val);
	if (key_present)
	{
		printf("The key \"%s\" should not be present (error!)\n", expected_zxcv_printable);
		return 1;
	}
	printf("Key deleted properly\n");

	json_object_put(parsed);
	printf("PASS\n");
	return 0;
}
