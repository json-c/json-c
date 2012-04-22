/*
* Tests if binary strings are supported.
*/

#include <stdio.h>
#include <string.h>
#include "config.h"

#include "json_inttypes.h"
#include "json_object.h"

int main() {
    // this test has a space after the null character. check that it's still included
    const char *input = " \0 ";
    const char *expected = "\" \\u0000 \"";
    struct json_object *string = json_object_new_string_len(input, 3);
    const char *json = json_object_to_json_string(string);

    int strings_match =  !strcmp( expected, json);
    int retval = 0;
    if (strings_match) {
        printf("JSON write result is correct: %s\n", json);
        printf("PASS\n");
    } else {
        printf("JSON write result doesn't match expected string\n");
        printf("expected string: ");
        printf("%s\n", expected);
        printf("parsed string:   ");
        printf("%s\n", json);
        printf("FAIL\n");
        retval=1;
    }
    json_object_put(string);
    return retval;
}
