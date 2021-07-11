#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "parse_flags.h"

int main(int argc, char **argv)
{
	struct json_object *new_obj;

	new_obj = json_tokener_parse("{ \"foo\\u0000bar\": \"qwerty\\u0000asdf\" }");
	printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
	json_object_put(new_obj);

	return EXIT_SUCCESS;
}
