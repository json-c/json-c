#ifdef NDEBUG
#undef NDEBUG
#endif
#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "json_object.h"
#include "linkhash.h"
#include "snprintf_compat.h"

/*
 * Keys that are added to and deleted from an object over and over leave
 * LH_FREED markers behind in its table.  Unless those count towards a
 * resize, the table eventually has no LH_EMPTY slots left and every lookup
 * of a missing key (including the one json_object_object_add() does before
 * each insert) walks the whole table.
 */
int main(void)
{
	struct json_object *object = json_object_new_object();
	struct lh_table *table;
	char key[32];
	int ii, empty;

	assert(object != NULL);
	for (ii = 0; ii < 100; ii++)
	{
		snprintf(key, sizeof(key), "k%d", ii);
		assert(json_object_object_add(object, key, NULL) == 0);
	}
	for (ii = 0; ii < 20000; ii++)
	{
		snprintf(key, sizeof(key), "k%d", ii + 100);
		assert(json_object_object_add(object, key, NULL) == 0);
		snprintf(key, sizeof(key), "k%d", ii);
		json_object_object_del(object, key);
	}

	/* The live keys are still there, in insertion order, and nothing else is */
	assert(json_object_object_length(object) == 100);
	for (ii = 0; ii < 20100; ii++)
	{
		snprintf(key, sizeof(key), "k%d", ii);
		assert(json_object_object_get_ex(object, key, NULL) == (ii >= 20000));
	}
	ii = 20000;
	{
		json_object_object_foreach(object, k, v)
		{
			(void)v;
			snprintf(key, sizeof(key), "k%d", ii++);
			assert(strcmp(k, key) == 0);
		}
	}

	table = json_object_get_object(object);
	empty = 0;
	for (ii = 0; ii < table->size; ii++)
	{
		if (table->table[ii].k == LH_EMPTY)
			empty++;
	}
	printf("empty slots left: %s\n", empty > 0 ? "yes" : "no");

	json_object_put(object);
	puts("PASS");
	return 0;
}
