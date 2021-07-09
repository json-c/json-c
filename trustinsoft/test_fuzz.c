#include <stdint.h>
#include <json.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2) exit(1);
  FILE *f = fopen(argv[1], "r");
  if (!f) exit(2);
  fseek(f, 0, SEEK_END);
  unsigned long len = (unsigned long)ftell(f);
  fseek(f, 0, SEEK_SET);

  char *input = malloc(len);
  if (!input) exit(3);

  size_t r = fread(input, len, 1, f);
  if (r != 1) exit(4);

  json_global_set_string_hash(JSON_C_STR_HASH_PERLLIKE);
  json_tokener *tok = json_tokener_new();
  json_object *obj = json_tokener_parse_ex(tok, input, len);

  json_object_put(obj);
  json_tokener_free(tok);
}
