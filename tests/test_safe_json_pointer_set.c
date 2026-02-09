#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "json.h"

static const char *input_json_str = "{ "
                                    "'foo': ['bar', 'baz'], "
                                    "'': 0, "
                                    "'a/b': 1, "
                                    "'c%d': 2, "
                                    "'e^f': 3, "
                                    "'g|h': 4, "
                                    "'i\\\\j': 5, "
                                    "'k\\\"l': 6, "
                                    "' ': 7, "
                                    "'m~n': 8 "
                                    "}";

static void test_example_set_with_limit_index(void)
{
    struct json_object *jo2, *jo1 = json_tokener_parse(input_json_str);
    size_t limit_index = 10;

    // testing if json_pointer_set_with_limit_index() works as json_pointer_set()
    assert(jo1 != NULL);
	printf("PASSED - SET_WITH_LIMIT - LOADED TEST JSON\n");
	printf("%s\n", json_object_get_string(jo1));

    assert(0 == json_pointer_set_with_limit_index(&jo1, "/foo/1", json_object_new_string("cod"), limit_index));
	assert(0 == strcmp("cod", json_object_get_string(json_object_array_get_idx(
	                              json_object_object_get(jo1, "foo"), 1))));
	printf("PASSED - SET_WITH_LIMIT - 'cod' in /foo/1\n");
	assert(0 != json_pointer_set_with_limit_index(&jo1, "/fud/gaw", (jo2 = json_tokener_parse("[1,2,3]")), limit_index));
	assert(errno == ENOENT);
	printf("PASSED - SET_WITH_LIMIT - non-existing /fud/gaw\n");
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/fud", json_object_new_object(), limit_index));
	printf("PASSED - SET_WITH_LIMIT - /fud == {}\n");
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/fud/gaw", jo2, limit_index)); /* re-using jo2 from above */
	printf("PASSED - SET_WITH_LIMIT - /fug/gaw == [1,2,3]\n");
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/fud/gaw/0", json_object_new_int(0), limit_index));
	assert(0 == json_pointer_setf(&jo1, json_object_new_int(0), "%s%s/%d", "/fud", "/gaw", 0));
	printf("PASSED - SET_WITH_LIMIT - /fug/gaw == [0,2,3]\n");
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/fud/gaw/-", json_object_new_int(4), limit_index));
	printf("PASSED - SET_WITH_LIMIT - /fug/gaw == [0,2,3,4]\n");
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/", json_object_new_int(9), limit_index));
	printf("PASSED - SET_WITH_LIMIT - / == 9\n");

	jo2 = json_tokener_parse(
	    "{ 'foo': [ 'bar', 'cod' ], '': 9, 'a/b': 1, 'c%d': 2, 'e^f': 3, 'g|h': 4, 'i\\\\j': "
	    "5, 'k\\\"l': 6, ' ': 7, 'm~n': 8, 'fud': { 'gaw': [ 0, 2, 3, 4 ] } }");
	assert(json_object_equal(jo2, jo1));
	printf("PASSED - SET_WITH_LIMIT - Final JSON is: %s\n", json_object_get_string(jo1));
	json_object_put(jo2);

	assert(0 == json_pointer_set_with_limit_index(&jo1, "", json_object_new_int(10), limit_index));
	assert(10 == json_object_get_int(jo1));
	printf("%s\n", json_object_get_string(jo1));

	json_object_put(jo1);

	jo1 = json_tokener_parse("[0, 1, 2, 3]");
	jo2 = json_tokener_parse("[0, 1, 2, 3, null, null, null, 7]");

	assert(0 == json_pointer_set_with_limit_index(&jo1, "/7", json_object_new_int(7), limit_index));
	assert(1 == json_object_equal(jo1, jo2));

	json_object_put(jo1);

	jo1 = json_tokener_parse("[0, 1, 2, 3]");

	assert(0 == json_pointer_setf(&jo1, json_object_new_int(7), "/%u", 7));
	assert(1 == json_object_equal(jo1, jo2));

	json_object_put(jo1);
	json_object_put(jo2);

    // testing with limit_index
    jo1 = json_tokener_parse("{'foo': ['bar', 'baz']}");
    jo2 = json_tokener_parse("{'foo': ['bar', 'cod']}");

    assert(0 == json_pointer_set_with_limit_index(&jo1, "/foo/1", json_object_new_string("cod"), limit_index));
    assert(json_object_equal(jo1, jo2));
    printf("PASSED - SET_LIMIT - Set value within limit (/foo/1 with limit 10)\n");

    assert(0 == json_pointer_set_with_limit_index(&jo1, "/bar", json_object_new_string("new_field"), limit_index));
    printf("PASSED - SET_LIMIT - Set value on an object (limit is ignored)\n");

    assert(0 == json_pointer_set_with_limit_index(&jo1, "/foo/20", json_object_new_string("big_index"), (size_t)-1));
    printf("PASSED - SET_LIMIT - Set value with limit_index = -1 (no limit)\n");

    json_object_put(jo1);
    json_object_put(jo2);
}

static void test_wrong_inputs_set_with_limit_index(void)
{
    struct json_object *jo2, *jo1 = json_tokener_parse(input_json_str);
    size_t limit_index = 10;

    // testing if json_pointer_set_with_limit_index() works as json_pointer_set()
    assert(jo1 != NULL);
	printf("PASSED - SET_WITH_LIMIT - LOADED TEST JSON\n");
	printf("%s\n", json_object_get_string(jo1));

	assert(0 != json_pointer_set_with_limit_index(NULL, NULL, NULL, limit_index));
	assert(0 != json_pointer_set_with_limit_index(&jo1, NULL, NULL, limit_index));
	printf("PASSED - SET_WITH_LIMIT - failed with NULL params for input json & path\n");

	assert(0 != json_pointer_set_with_limit_index(&jo1, "foo/bar", (jo2 = json_object_new_string("cod")), limit_index));
	printf("PASSED - SET_WITH_LIMIT - failed 'cod' with path 'foo/bar'\n");
	json_object_put(jo2);

	// assert(0 !=
	//        json_pointer_setf(&jo1, (jo2 = json_object_new_string("cod")), "%s", "foo/bar"));
	// printf("PASSED - SET_WITH_LIMIT - failed 'cod' with path 'foo/bar'\n");
	// json_object_put(jo2);

	assert(0 != json_pointer_set_with_limit_index(&jo1, "0", (jo2 = json_object_new_string("cod")), limit_index));
	printf("PASSED - SET_WITH_LIMIT - failed with invalid array index'\n");
	json_object_put(jo2);

	jo2 = json_object_new_string("whatever");
	assert(0 != json_pointer_set_with_limit_index(&jo1, "/fud/gaw", jo2, limit_index));
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/fud", json_object_new_object(), limit_index));
	assert(0 == json_pointer_set_with_limit_index(&jo1, "/fud/gaw", jo2, limit_index)); /* re-using jo2 from above */
	// ownership of jo2 transferred into jo1

	jo2 = json_object_new_int(0);
	assert(0 != json_pointer_set_with_limit_index(&jo1, "/fud/gaw/0", jo2, limit_index));
	json_object_put(jo2);
	jo2 = json_object_new_int(0);
	assert(0 != json_pointer_set_with_limit_index(&jo1, "/fud/gaw/", jo2, limit_index));
	json_object_put(jo2);
	printf("PASSED - SET_WITH_LIMIT - failed to set index to non-array\n");

	// assert(0 == json_pointer_setf(&jo1, json_object_new_string("cod"), "%s", "\0"));

	json_object_put(jo1);

    // testing with limit_index
    jo1 = json_tokener_parse("{'foo': ['bar', 'baz']}");

    errno = 0;
    jo2 = json_object_new_string("out_of_bounds");
    assert(0 != json_pointer_set_with_limit_index(&jo1, "/foo/20", jo2, limit_index));
    assert(errno == EINVAL);
    printf("PASSED - SET_LIMIT - Failed to set index 20 with limit 10\n");
    // The value object was not consumed, so we must put it.
    json_object_put(jo2);

    // corner case: setting an index that equals the limit (should be ok, as it's idx > max_idx)
    errno = 0;
    assert(0 == json_pointer_set_with_limit_index(&jo1, "/foo/10", json_object_new_string("at_the_limit"), limit_index));
    printf("PASSED - SET_LIMIT - Succeeded to set index 10 with limit 10\n");

    json_object_put(jo1);
}

// callback for testing json_pointer_set_with_cb()
static int test_cb_print_msg(json_object *parent, const char *key, size_t idx,
                            json_object *value, void *priv)
{
    printf("PASSED - SET_WITH_CB - This callback is called\n");
    return 0;
}

// callback for testing json_pointer_set_with_cb() with rejection logic
static int test_cb_reject_logic(json_object *parent, const char *key, size_t idx,
                            	json_object *value, void *priv)
{
    // Reject any operation if the key is "reject"
    if (key && strcmp(key, "reject") == 0)
    {
        printf("PASSED - SET_WITH_CB - Callback correctly identified key 'reject' to reject\n");
        return -1;
    }
    return 0;
}

static void test_set_with_cb(void)
{
    struct json_object *jo1 = json_tokener_parse(input_json_str);
    size_t limit_index = 5;

	assert(jo1 != NULL);
	printf("PASSED - SET_WITH_CB - LOADED TEST JSON\n");
	printf("%s\n", json_object_get_string(jo1));

    assert(0 == json_pointer_set_with_cb(&jo1, "/foo/1", json_object_new_string("cod"), test_cb_print_msg, 1, NULL));
    printf("PASSED - SET_WITH_CB - callback test_cb_print_msg for /foo/1\n");

	assert(0 == json_pointer_set_with_cb(&jo1, "/foo/4", json_object_new_string("in"), json_object_array_put_with_idx_limit_cb, 0, &limit_index));
    printf("PASSED - SET_WITH_CB - callback json_object_array_put_with_idx_limit_cb for /foo/4 with limit_index 5\n");

    assert(0 == json_pointer_set_with_cb(&jo1, "/foo/5", json_object_new_string("border"), json_object_array_put_with_idx_limit_cb, 0, &limit_index));
    printf("PASSED - SET_WITH_CB - failed with callback json_object_array_put_with_idx_limit_cb for /foo/5 with limit_index 5\n");

    assert(0 != json_pointer_set_with_cb(&jo1, "/foo/10", json_object_new_string("out"), json_object_array_put_with_idx_limit_cb, 0, &limit_index));
    assert(errno == EINVAL);
    printf("PASSED - SET_WITH_CB - failed with callback json_object_array_put_with_idx_limit_cb for /foo/10 with limit_index 5\n");

    assert(0 != json_pointer_set_with_cb(&jo1, "/foo/2", json_object_new_string("null_priv"), json_object_array_put_with_idx_limit_cb, 0, NULL));
    assert(errno == EFAULT);
    printf("PASSED - SET_WITH_CB - failed with callback json_object_array_put_with_idx_limit_cb with NULL priv\n");

	json_object_put(jo1);

	jo1 = json_tokener_parse("{'foo': 'bar'}");
    assert(jo1 != NULL);

	assert(0 == json_pointer_set_with_cb(&jo1, "/foo", json_object_new_string("cod"), test_cb_print_msg, 1, NULL));
    printf("PASSED - SET_WITH_CB - cb_handles_obj=1: callback was triggered for object operation\n");

	assert(0 == json_pointer_set_with_cb(&jo1, "/new_key", json_object_new_string("new_value"), test_cb_print_msg, 0, NULL));
    printf("PASSED - SET_WITH_CB - cb_handles_obj=0: callback was NOT triggered for object operation, default logic was used\n");
    json_object_put(jo1);

	// testing rejection logic callback
    jo1 = json_tokener_parse("{'data': {} }");
    assert(jo1 != NULL);

    assert(0 == json_pointer_set_with_cb(&jo1, "/data/accept", json_object_new_string("accepted_value"), test_cb_reject_logic, 1, NULL));
    printf("PASSED - SET_WITH_CB - Rejection callback approved an allowed key\n");

    assert(0 != json_pointer_set_with_cb(&jo1, "/data/reject", json_object_new_string("rejected_value"), test_cb_reject_logic, 1, NULL));
    printf("PASSED - SET_WITH_CB - Rejection callback rejected a forbidden key\n");

    json_object_put(jo1);
}

int main(int argc, char **argv)
{
    test_example_set_with_limit_index();
    test_wrong_inputs_set_with_limit_index();
    test_set_with_cb();
	return 0;
}
