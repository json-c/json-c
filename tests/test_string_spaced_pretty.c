#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "json.h"

int main(int argc, char **argv)
{
	json_object *j;

	j = json_tokener_parse("/* more difficult test case */"
				     "{ \"glossary\": { \"title\": \"example glossary\", \"GlossDiv\": { \"title\": \"S\", \"GlossList\": [ { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", \"GlossDef\": \"A meta-markup language, used to create markup languages such as DocBook.\", \"GlossSeeAlso\": [\"GML\", \"XML\", \"markup\"] } ] } } }");

	printf("flags = 0:\n%s\n\n",
			json_object_to_json_string_ext(j, 0));

	printf("flags = JSON_C_TO_STRING_SPACED:\n%s\n\n",
			json_object_to_json_string_ext(j, JSON_C_TO_STRING_SPACED));

	printf("flags = JSON_C_TO_STRING_SPACED|JSON_C_TO_STRING_PRETTY:\n%s\n\n",
			json_object_to_json_string_ext(j, JSON_C_TO_STRING_SPACED
												|JSON_C_TO_STRING_PRETTY));

	printf("flags = JSON_C_TO_STRING_SPACED|JSON_C_TO_STRING_PRETTY|JSON_C_TO_STRING_PRETTY_TAB:\n%s\n",
			json_object_to_json_string_ext(j, JSON_C_TO_STRING_SPACED
												|JSON_C_TO_STRING_PRETTY
												|JSON_C_TO_STRING_PRETTY_TAB));

	json_object_put(j);

	return EXIT_SUCCESS;
}
