#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include "json.h"

static const char *json_str1 = 
"{"
"    \"glossary\": {"
"        \"title\": \"example glossary\","
"        \"GlossDiv\": {"
"            \"title\": \"S\","
"            \"null_obj\": null, "
"            \"GlossList\": {"
"                \"GlossEntry\": {"
"                    \"ID\": \"SGML\","
"                    \"SortAs\": \"SGML\","
"                    \"GlossTerm\": \"Standard Generalized Markup Language\","
"                    \"Acronym\": \"SGML\","
"                    \"Abbrev\": \"ISO 8879:1986\","
"                    \"GlossDef\": {"
"                        \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\","
"                        \"GlossSeeAlso\": [\"GML\", \"XML\"]"
"                    },"
"                    \"GlossSee\": \"markup\""
"                }"
"            }"
"        }"
"    }"
"}";

static const char *json_str2 =
"{\"menu\": {"
"    \"header\": \"SVG Viewer\","
"    \"items\": ["
"        {\"id\": \"Open\"},"
"        {\"id\": \"OpenNew\", \"label\": \"Open New\"},"
"        null,"
"        {\"id\": \"ZoomIn\", \"label\": \"Zoom In\"},"
"        {\"id\": \"ZoomOut\", \"label\": \"Zoom Out\"},"
"        {\"id\": \"OriginalView\", \"label\": \"Original View\"},"
"        null,"
"        {\"id\": \"Quality\", \"another_null\": null},"
"        {\"id\": \"Pause\"},"
"        {\"id\": \"Mute\"},"
"        null,"
"        {\"id\": \"Find\", \"label\": \"Find...\"},"
"        {\"id\": \"FindAgain\", \"label\": \"Find Again\"},"
"        {\"id\": \"Copy\"},"
"        {\"id\": \"CopyAgain\", \"label\": \"Copy Again\"},"
"        {\"id\": \"CopySVG\", \"label\": \"Copy SVG\"},"
"        {\"id\": \"ViewSVG\", \"label\": \"View SVG\"},"
"        {\"id\": \"ViewSource\", \"label\": \"View Source\"},"
"        {\"id\": \"SaveAs\", \"label\": \"Save As\"},"
"        null,"
"        {\"id\": \"Help\"},"
"        {\"id\": \"About\", \"label\": \"About Adobe CVG Viewer...\"}"
"    ]"
"}}";

static const char *json_str3 =
"{\"menu\": {"
"  \"id\": \"file\","
"  \"value\": \"File\","
"  \"popup\": {"
"    \"menuitem\": ["
"      {\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},"
"      {\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},"
"      {\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}"
"    ]"
"  }"
"}}";

int main(int argc, char **argv)
{
	struct json_object *src1, *src2, *src3;
	struct json_object *dst1 = NULL, *dst2 = NULL, *dst3 = NULL;

	src1 = json_tokener_parse(json_str1);
	src2 = json_tokener_parse(json_str2);
	src3 = json_tokener_parse(json_str3);

	assert(src1 != NULL);
	assert(src1 != NULL);
	assert(src3 != NULL);

	printf("PASSED - loaded input data\n");

	/* do this 3 times to make sure overwriting it works */
	assert(0 == json_object_deep_copy(src1, &dst1));
	assert(0 == json_object_deep_copy(src2, &dst2));
	assert(0 == json_object_deep_copy(src3, &dst3));

	printf("PASSED - all json_object_deep_copy() returned succesful\n");

	assert(-1 == json_object_deep_copy(src1, &dst1));
	assert(errno == EINVAL);
	assert(-1 == json_object_deep_copy(src2, &dst2));
	assert(errno == EINVAL);
	assert(-1 == json_object_deep_copy(src3, &dst3));
	assert(errno == EINVAL);

	printf("PASSED - all json_object_deep_copy() returned EINVAL for non-null pointer\n");

	assert(1 == json_object_equal(src1, dst1));
	assert(1 == json_object_equal(src2, dst2));
	assert(1 == json_object_equal(src3, dst3));

	printf("PASSED - all json_object_equal() tests returned succesful\n");

	assert(0 == strcmp(json_object_to_json_string_ext(src1, JSON_C_TO_STRING_PRETTY),
	                   json_object_to_json_string_ext(dst1, JSON_C_TO_STRING_PRETTY)));
	assert(0 == strcmp(json_object_to_json_string_ext(src2, JSON_C_TO_STRING_PRETTY),
	                   json_object_to_json_string_ext(dst2, JSON_C_TO_STRING_PRETTY)));
	assert(0 == strcmp(json_object_to_json_string_ext(src3, JSON_C_TO_STRING_PRETTY),
	                   json_object_to_json_string_ext(dst3, JSON_C_TO_STRING_PRETTY)));

	printf("PASSED - comparison of string output\n");

	json_object_get(dst1);
	assert(-1 == json_object_deep_copy(src1, &dst1));
	assert(errno == EINVAL);

	printf("PASSED - trying to overrwrite an object that has refcount > 1");

	printf("\nPrinting JSON objects for visual inspection\n");
	printf("------------------------------------------------\n");
	printf(" JSON1\n");
	printf("%s", json_object_to_json_string_ext(dst1, JSON_C_TO_STRING_PRETTY));
	printf("------------------------------------------------\n");

	printf("------------------------------------------------\n");
	printf(" JSON2\n");
	printf("%s", json_object_to_json_string_ext(dst2, JSON_C_TO_STRING_PRETTY));
	printf("------------------------------------------------\n");

	printf("------------------------------------------------\n");
	printf(" JSON3\n");
	printf("------------------------------------------------\n");
	printf("%s", json_object_to_json_string_ext(dst3, JSON_C_TO_STRING_PRETTY));
	printf("------------------------------------------------\n");

	json_object_put(dst1);
	json_object_put(dst2);
	json_object_put(dst3);

#if BENCHMARK
	/**
	 * The numbers that I got are:
	 * BENCHMARK - 1000000 iterations of 'dst2 = json_tokener_parse(json_object_get_string(src2))' took 20 seconds
	 * BENCHMARK - 1000000 iterations of 'dst2 = json_tokener_parse(json_object_get_string(src2))' took 7 seconds
	 */

	int iterations = 1000000;
	time_t start = time(NULL);
	for (i = 0; i < iterations; i++) {
		dst2 = json_tokener_parse(json_object_get_string(src2));
		json_object_put(dst2);
	}
	printf("BENCHMARK - %d iterations of 'dst2 = json_tokener_parse(json_object_get_string(src2))' took %d seconds\n", iterations, (int)(time(NULL) - start));

	start = time(NULL);
	for (i = 0; i < iterations; i++) {
		json_object_deep_copy(src2, &dst2);
	}
	json_object_put(dst2);
	printf("BENCHMARK - %d iterations of 'dst2 = json_tokener_parse(json_object_get_string(src2))' took %d seconds\n", iterations, (int)(time(NULL) - start));
#endif

	return 0;
}
