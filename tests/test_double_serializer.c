/*
* Tests if the format string for double serialization is handled correctly
*/

#include <stdio.h>
#include "config.h"

#include "json_object.h"
#include "json_object_private.h"

int main()
{
	struct json_object *obj = json_object_new_double(0.5);

	printf("Test default serializer:\n");
	printf("obj.to_string(standard)=%s\n", json_object_to_json_string(obj));

	printf("Test default serializer with custom userdata:\n");
	obj->_userdata = "test";
	printf("obj.to_string(userdata)=%s\n", json_object_to_json_string(obj));

	printf("Test explicit serializer with custom userdata:\n");
	json_object_set_serializer(obj, json_object_double_to_json_string, "test", NULL);
	printf("obj.to_string(custom)=%s\n", json_object_to_json_string(obj));

	printf("Test reset serializer:\n");
	json_object_set_serializer(obj, NULL, NULL, NULL);
	printf("obj.to_string(reset)=%s\n", json_object_to_json_string(obj));

	json_object_put(obj);
}
