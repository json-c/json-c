#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "json.h"

static int sort_fn (const void *j1, const void *j2)
{
  json_object **jso1, **jso2;
  int i1, i2;

  jso1 = j1;
  jso2 = j2;
  if (!*jso1 && !*jso2) {
    return 0;
  }
  if (!*jso1) {
    return -1;
  }
  if (!*jso2) {
    return 1;
  }

  i1 = json_object_get_int(*jso1);
  i2 = json_object_get_int(*jso2);

  return i1 - i2;
}

int main(int argc, char **argv)
{
  json_tokener *tok;
  json_object *my_string, *my_int, *my_object, *my_array;
  json_object *new_obj;
  int i;

  MC_SET_DEBUG(1);

  my_string = json_object_new_string("\t");
  printf("my_string=%s\n", json_object_get_string(my_string));
  printf("my_string.to_string()=%s\n", json_object_to_json_string(my_string));
  json_object_put(my_string);

  my_string = json_object_new_string("\\");
  printf("my_string=%s\n", json_object_get_string(my_string));
  printf("my_string.to_string()=%s\n", json_object_to_json_string(my_string));
  json_object_put(my_string);

  my_string = json_object_new_string("foo");
  printf("my_string=%s\n", json_object_get_string(my_string));
  printf("my_string.to_string()=%s\n", json_object_to_json_string(my_string));

  my_int = json_object_new_int(9);
  printf("my_int=%d\n", json_object_get_int(my_int));
  printf("my_int.to_string()=%s\n", json_object_to_json_string(my_int));

  my_array = json_object_new_array();
  json_object_array_add(my_array, json_object_new_int(1));
  json_object_array_add(my_array, json_object_new_int(2));
  json_object_array_add(my_array, json_object_new_int(3));
  json_object_array_put_idx(my_array, 4, json_object_new_int(5));
  printf("my_array=\n");
  for(i=0; i < json_object_array_length(my_array); i++) {
    json_object *obj = json_object_array_get_idx(my_array, i);
    printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
  }
  printf("my_array.to_string()=%s\n", json_object_to_json_string(my_array));    

  json_object_put(my_array);

  my_array = json_object_new_array();
  json_object_array_add(my_array, json_object_new_int(3));
  json_object_array_add(my_array, json_object_new_int(1));
  json_object_array_add(my_array, json_object_new_int(2));
  json_object_array_put_idx(my_array, 4, json_object_new_int(0));
  printf("my_array=\n");
  for(i=0; i < json_object_array_length(my_array); i++) {
    json_object *obj = json_object_array_get_idx(my_array, i);
    printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
  }
  printf("my_array.to_string()=%s\n", json_object_to_json_string(my_array));    
  json_object_array_sort(my_array, sort_fn);
  printf("my_array=\n");
  for(i=0; i < json_object_array_length(my_array); i++) {
    json_object *obj = json_object_array_get_idx(my_array, i);
    printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
  }
  printf("my_array.to_string()=%s\n", json_object_to_json_string(my_array));    

  my_object = json_object_new_object();
  json_object_object_add(my_object, "abc", json_object_new_int(12));
  json_object_object_add(my_object, "foo", json_object_new_string("bar"));
  json_object_object_add(my_object, "bool0", json_object_new_boolean(0));
  json_object_object_add(my_object, "bool1", json_object_new_boolean(1));
  json_object_object_add(my_object, "baz", json_object_new_string("bang"));
  json_object_object_add(my_object, "baz", json_object_new_string("fark"));
  json_object_object_del(my_object, "baz");
  /*json_object_object_add(my_object, "arr", my_array);*/
  printf("my_object=\n");
  json_object_object_foreach(my_object, key, val) {
    printf("\t%s: %s\n", key, json_object_to_json_string(val));
  }
  printf("my_object.to_string()=%s\n", json_object_to_json_string(my_object));

  new_obj = json_tokener_parse("\"\003\"");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("/* hello */\"foo\"");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("// hello\n\"foo\"");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("\"\\u0041\\u0042\\u0043\"");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("null");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("True");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("12");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("12.3");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("[\"\\n\"]");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("[\"\\nabc\\n\"]");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("[null]");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("[]");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("[false]");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("[\"abc\",null,\"def\",12]");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("{}");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("{ \"foo\": \"bar\" }");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("{ \"foo\": \"bar\", \"baz\": null, \"bool0\": true }");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("{ \"foo\": [null, \"foo\"] }");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  new_obj = json_tokener_parse("{ \"abc\": 12, \"foo\": \"bar\", \"bool0\": false, \"bool1\": true, \"arr\": [ 1, 2, 3, null, 5 ] }");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);

  enum json_tokener_error error = json_tokener_success;
  new_obj = json_tokener_parse_verbose("{ foo }", &error);
  assert (error == json_tokener_error_parse_object_key_name);
  assert (new_obj == NULL);

  new_obj = json_tokener_parse("{ foo }");
  assert (new_obj == NULL);
  
  // if(is_error(new_obj)) printf("got error as expected\n");

  new_obj = json_tokener_parse("foo");
  assert (new_obj == NULL);
  new_obj = json_tokener_parse_verbose("foo", &error);
  assert (new_obj == NULL);
  assert (error == json_tokener_error_parse_boolean);

  new_obj = json_tokener_parse("{ \"foo");
  if(is_error(new_obj)) printf("got error as expected\n");

  /* test incremental parsing */
  tok = json_tokener_new();
  new_obj = json_tokener_parse_ex(tok, "{ \"foo", 6);
  if(is_error(new_obj)) printf("got error as expected\n");
  new_obj = json_tokener_parse_ex(tok, "\": {\"bar", 8);
  if(is_error(new_obj)) printf("got error as expected\n");
  new_obj = json_tokener_parse_ex(tok, "\":13}}", 6);
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  json_object_put(new_obj);  
  json_tokener_free(tok);

  json_object_put(my_string);
  json_object_put(my_int);
  json_object_put(my_object);
  json_object_put(my_array);

  return 0;
}
