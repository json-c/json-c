#ifdef NDEBUG
#undef NDEBUG
#endif
#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_object.h"
#include "json_object_iterator.h"
#include "json_tokener.h"

int main(int atgc, char **argv)
{
	const char *input = "{\n\
		\"string_of_digits\": \"123\",\n\
		\"regular_number\": 222,\n\
		\"decimal_number\": 99.55,\n\
		\"boolean_true\": true,\n\
		\"boolean_false\": false,\n\
		\"big_number\": 2147483649,\n\
		\"a_null\": null,\n\
                \"an_object\": { \"foo\": \"bar\" },\n\
		}";

	struct json_object *new_obj;
	struct json_object_iterator it;
	struct json_object_iterator itEnd;

	it = json_object_iter_init_default();
	new_obj = json_tokener_parse(input);
	it = json_object_iter_begin(new_obj);
	itEnd = json_object_iter_end(new_obj);

	// If this changes size on any platform, it'd break the ABI
	printf("sizeof(struct json_object_iterator)/sizeof(void *)=%zu\n", sizeof(struct json_object_iterator) / sizeof(void *));

	while (!json_object_iter_equal(&it, &itEnd))
	{
		const char *key = json_object_iter_peek_name(&it);
		json_object *value = json_object_iter_peek_value(&it);
		printf("key: %s\n", key);
		printf("pre change:  %s\n", json_object_to_json_string(value));
		if (json_object_get_type(value) == json_type_object)
		{
			// Changing sub-objects is ok
			assert(json_object_object_add(value, "foo", json_object_new_string("change foo")) == 0);
			assert(json_object_object_add(value, "newkey", json_object_new_string("new value")) == 0);
		}
		else
		{
			// Changing the value of pairs is ok
			assert(json_object_object_add(new_obj, key, json_object_new_string("switched")) == 0);
		}

		if (0) // DO NOT DO THIS
		{
			// Adding new keys is NOT ok, and invalidates the iterator
			json_object_object_add(value, "somenewkey", json_object_new_string("foo"));

			// Deleting any keys ALSO invalidates the iterator, even if
			// the key is immediately re-added.
			json_object_object_del(value, key);
			json_object_object_add(value, key, json_object_new_string("switched"));
		}

		value = json_object_iter_peek_value(&it);
		printf("post change: %s\n", json_object_to_json_string(value));

		json_object_iter_next(&it);
	}

	json_object_put(new_obj);

	return 0;
}
