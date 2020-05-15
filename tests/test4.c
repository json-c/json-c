/*
 * gcc -o utf8 utf8.c -I/home/y/include -L./.libs -ljson
*/

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_inttypes.h"
#include "json_object.h"
#include "json_object_private.h"
#include "json_tokener.h"
#include "linkhash.h"

void print_hex( const char* s) {
        const char *iter = s;
        unsigned char ch;
        while ((ch = *iter++) != 0) {
           if( ',' != ch)
            printf("%x ", ch);
           else
            printf( ",");
        }
        printf("\n");
}

static void test_lot_of_adds(void);
static void test_lot_of_adds()
{
    int ii;
    char key[50];
    json_object *jobj = json_object_new_object();
    assert(jobj != NULL);
    for (ii = 0; ii < 500; ii++) {
        snprintf(key, sizeof(key), "k%d", ii);
        json_object *iobj = json_object_new_int(ii);
        assert(iobj != NULL);
        json_object_object_add(jobj, key, iobj);
        if (json_object_object_get_ex(jobj, key, &iobj) == FALSE) {
            fprintf(stderr, "FAILED to add object #%d\n", ii);
            abort();
        }
    }
    printf("%s\n", json_object_to_json_string(jobj));
    assert(jobj->o.c_object->count == 500);
    json_object_put(jobj);
}

int main() {
    const char *input = "\"\\ud840\\udd26,\\ud840\\udd27,\\ud800\\udd26,\\ud800\\udd27\"";
    const char *expected = "\xF0\xA0\x84\xA6,\xF0\xA0\x84\xA7,\xF0\x90\x84\xA6,\xF0\x90\x84\xA7";
    struct json_object *parse_result = json_tokener_parse((char*)input);
    const char *unjson = json_object_get_string(parse_result);

    printf("input: %s\n", input);

    int strings_match = !strcmp( expected, unjson);
	int retval = 0;
    if (strings_match) {
        printf("JSON parse result is correct: %s\n", unjson);
        printf("PASS\n");
    } else {
        printf("JSON parse result doesn't match expected string\n");
        printf("expected string bytes: ");
        print_hex( expected);
        printf("parsed string bytes:   ");
        print_hex( unjson);
        printf("FAIL\n");
        retval = 1;
    }
	json_object_put(parse_result);

	test_lot_of_adds();

	return retval;
}
