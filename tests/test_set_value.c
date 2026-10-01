#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "printbuf.h"

int main(int argc, char **argv)
{
	json_object *tmp = json_object_new_int(123);
	assert(json_object_get_int(tmp) == 123);
	json_object_set_int(tmp, 321);
	assert(json_object_get_int(tmp) == 321);
	printf("INT PASSED\n");
	json_object_set_int64(tmp, (int64_t)321321321);
	assert(json_object_get_int64(tmp) == 321321321);
	json_object_put(tmp);
	printf("INT64 PASSED\n");
	tmp = json_object_new_uint64(123);
	assert(json_object_get_boolean(tmp) == 1);
	assert(json_object_get_int(tmp) == 123);
	assert(json_object_get_int64(tmp) == 123);
	assert(json_object_get_uint64(tmp) == 123);
	assert(json_object_get_double(tmp) == 123.000000);
	json_object_set_uint64(tmp, (uint64_t)321321321);
	assert(json_object_get_uint64(tmp) == 321321321);
	json_object_set_uint64(tmp, 9223372036854775808U);
	assert(json_object_get_int(tmp) == INT32_MAX);
	assert(json_object_get_uint64(tmp) == 9223372036854775808U);
	json_object_put(tmp);
	printf("UINT64 PASSED\n");
	tmp = json_object_new_boolean(1);
	assert(json_object_get_boolean(tmp) == 1);
	json_object_set_boolean(tmp, 0);
	assert(json_object_get_boolean(tmp) == 0);
	json_object_set_boolean(tmp, 1);
	assert(json_object_get_boolean(tmp) == 1);
	json_object_put(tmp);
	printf("BOOL PASSED\n");
	tmp = json_object_new_double(12.34);
	assert(json_object_get_double(tmp) == 12.34);
	json_object_set_double(tmp, 34.56);
	assert(json_object_get_double(tmp) == 34.56);
	json_object_set_double(tmp, 6435.34);
	assert(json_object_get_double(tmp) == 6435.34);
	json_object_set_double(tmp, 2e21);
	assert(json_object_get_int(tmp) == INT32_MAX);
	assert(json_object_get_int64(tmp) == INT64_MAX);
	assert(json_object_get_uint64(tmp) == UINT64_MAX);
	json_object_set_double(tmp, -2e21);
	assert(json_object_get_int(tmp) == INT32_MIN);
	assert(json_object_get_int64(tmp) == INT64_MIN);
	assert(json_object_get_uint64(tmp) == 0);
	json_object_put(tmp);
	printf("DOUBLE PASSED\n");
#define SHORT "SHORT"
#define MID "A MID STRING"
//             12345678901234567890123456789012....
#define HUGE "A string longer than 32 chars as to check non local buf codepath"
	tmp = json_object_new_string(MID);
	assert(strcmp(json_object_get_string(tmp), MID) == 0);
	assert(strcmp(json_object_to_json_string(tmp), "\"" MID "\"") == 0);
	json_object_set_string(tmp, SHORT);
	assert(strcmp(json_object_get_string(tmp), SHORT) == 0);
	assert(strcmp(json_object_to_json_string(tmp), "\"" SHORT "\"") == 0);
	json_object_set_string(tmp, HUGE);
	assert(strcmp(json_object_get_string(tmp), HUGE) == 0);
	assert(strcmp(json_object_to_json_string(tmp), "\"" HUGE "\"") == 0);
	json_object_set_string(tmp, SHORT);
	assert(strcmp(json_object_get_string(tmp), SHORT) == 0);
	assert(strcmp(json_object_to_json_string(tmp), "\"" SHORT "\"") == 0);

	// Set an empty string a couple times to try to trigger
	// a case that used to leak memory.
	json_object_set_string(tmp, "");
	json_object_set_string(tmp, HUGE);
	json_object_set_string(tmp, "");
	json_object_set_string(tmp, HUGE);

	json_object_put(tmp);
	printf("STRING PASSED\n");

#define STR "STR"
#define DOUBLE "123.123"
#define DOUBLE_E "12E+3"
#define DOUBLE_STR "123.123STR"
#define DOUBLE_OVER "1.8E+308"
#define DOUBLE_OVER_NEGATIVE "-1.8E+308"
	tmp = json_object_new_string(STR);
	assert(json_object_get_double(tmp) == 0.0);
	json_object_set_string(tmp, DOUBLE);
	assert(json_object_get_double(tmp) == 123.123000);
	json_object_set_string(tmp, DOUBLE_E);
	assert(json_object_get_double(tmp) == 12000.000000);
	json_object_set_string(tmp, DOUBLE_STR);
	assert(json_object_get_double(tmp) == 0.0);
	json_object_set_string(tmp, DOUBLE_OVER);
	assert(json_object_get_double(tmp) == 0.0);
	json_object_set_string(tmp, DOUBLE_OVER_NEGATIVE);
	assert(json_object_get_double(tmp) == 0.0);
	json_object_put(tmp);
	printf("STRINGTODOUBLE PASSED\n");

	tmp = json_tokener_parse("1.234");
	json_object_set_double(tmp, 12.3);
	const char *serialized = json_object_to_json_string(tmp);
	if (getenv("JSONC_TEST_TRACE") != NULL)
		// This output might be different on different systems
		fprintf(stderr, "%s\n", serialized);
	assert(strncmp(serialized, "12.3", 4) == 0);
	json_object_put(tmp);
	printf("PARSE AND SET PASSED\n");

	/* Defensive NULL and boundary checks */
	assert(json_object_new_string(NULL) == NULL);
	assert(json_object_new_string_len(NULL, 10) == NULL);
	assert(json_object_new_string_len("abc", -1) == NULL);
	assert(json_object_set_string(NULL, "test") == 0);
	assert(json_object_set_string_len(NULL, "test", 4) == 0);

	struct json_object *str_obj = json_object_new_string("hello");
	assert(str_obj != NULL);
	assert(json_object_set_string(str_obj, NULL) == 0);
	assert(json_object_set_string_len(str_obj, NULL, 5) == 0);
	assert(json_object_set_string_len(str_obj, "world", -1) == 0);
	json_object_set_userdata(NULL, NULL, NULL);
	json_object_set_serializer(NULL, NULL, NULL, NULL);
	json_object_put(str_obj);

	/* Object and array NULL checks */
	assert(json_object_object_length(NULL) == 0);
	assert(json_object_object_add(NULL, "k", NULL) == -1);
	json_object_object_del(NULL, "k");
	assert(json_object_array_length(NULL) == 0);
	assert(json_object_array_add(NULL, NULL) == -1);
	assert(json_object_array_get_idx(NULL, 0) == NULL);
	assert(json_object_array_insert_idx(NULL, 0, NULL) == -1);
	assert(json_object_array_put_idx(NULL, 0, NULL) == -1);
	assert(json_object_array_del_idx(NULL, 0, 1) == -1);
	assert(json_object_array_shrink(NULL, 0) == -1);
	json_object_array_sort(NULL, NULL);
	assert(json_object_array_bsearch(NULL, NULL, NULL) == NULL);

	/* Iterator NULL checks */
	struct json_object_iterator it = json_object_iter_begin(NULL);
	struct json_object_iterator it_end = json_object_iter_end(NULL);
	assert(json_object_iter_equal(&it, &it_end));
	assert(json_object_iter_peek_name(&it) == NULL);
	assert(json_object_iter_peek_value(&it) == NULL);
	json_object_iter_next(&it);

	/* Printbuf NULL checks */
	assert(printbuf_memappend(NULL, "abc", 3) == -1);
	assert(printbuf_memset(NULL, 0, 'a', 5) == -1);
	assert(sprintbuf(NULL, "test") == -1);
	printbuf_reset(NULL);

	/* Util NULL checks */
	double d;
	int64_t i64;
	uint64_t u64;
	assert(json_parse_double(NULL, &d) != 0);
	assert(json_parse_int64(NULL, &i64) != 0);
	assert(json_parse_uint64(NULL, &u64) != 0);
	assert(json_object_from_file(NULL) == NULL);
	assert(json_object_to_file_ext(NULL, NULL, 0) == -1);

	printf("PASSED\n");
	return 0;
}
