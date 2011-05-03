/*
 * Tests if casting within the json_object_get_* functions work correctly.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"
#include "json_util.h"

static void getit(struct json_object *new_obj, const char *field);

int main(int argc, char **argv)
{
	const char *input = "{\n\
		\"string_of_digits\": \"123\",\n\
		\"regular_number\": 222,\n\
		\"decimal_number\": 99.55,\n\
		\"boolean_true\": true,\n\
		\"boolean_false\": false,\n\
		\"big_number\": 2147483649,\n\
	}";
	/* Note: 2147483649 = INT_MAX + 2 */

	struct json_object *new_obj;

	new_obj = json_tokener_parse(input);
	printf("Parsed input: %s\n", input);
	printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
	if (!new_obj)
		return 1; // oops, we failed.

	getit(new_obj, "string_of_digits");
	getit(new_obj, "regular_number");
	getit(new_obj, "decimal_number");
	getit(new_obj, "boolean_true");
	getit(new_obj, "boolean_false");
	getit(new_obj, "big_number");

    json_object_put(new_obj);

    return 0;
}

static void getit(struct json_object *new_obj, const char *field)
{
	struct json_object *o = json_object_object_get(new_obj, field);

	enum json_type o_type = json_object_get_type(o);
	printf("new_obj.%s json_object_get_type()=%s\n", field,
	       json_type_to_name(o_type));
	printf("new_obj.%s json_object_get_int()=%d\n", field,
	       json_object_get_int(o));
	printf("new_obj.%s json_object_get_int64()=%" PRId64 "\n", field,
	       json_object_get_int64(o));
	printf("new_obj.%s json_object_get_boolean()=%d\n", field,
	       json_object_get_boolean(o));
	printf("new_obj.%s json_object_get_double()=%f\n", field,
	       json_object_get_double(o));
}
