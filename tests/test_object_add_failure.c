#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "json.h"

static void check_insert_failure(const char *key, unsigned int opts)
{
	struct json_object *object = json_object_new_object();
	struct json_object *value = json_object_new_int(7);
	struct lh_table *table;
	int saved_count;
	int saved_size;

	assert(object != NULL);
	assert(value != NULL);
	table = json_object_get_object(object);
	assert(table != NULL);

	/* Force lh_table_insert_w_hash() to reject the new entry. */
	saved_count = table->count;
	saved_size = table->size;
	table->count = INT_MAX;
	table->size = INT_MAX;

	assert(json_object_object_add_ex(object, key, value,
	                                 opts | JSON_C_OBJECT_ADD_KEY_IS_NEW) == -1);

	table->count = saved_count;
	table->size = saved_size;
	json_object_put(value);
	json_object_put(object);
}

int main(void)
{
	check_insert_failure("copied-key", 0);
	check_insert_failure("constant-key", JSON_C_OBJECT_ADD_CONSTANT_KEY);
	puts("PASS");
	return 0;
}
