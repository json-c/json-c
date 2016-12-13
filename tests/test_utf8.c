#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

int main() {
	const char inputs[][20] = {
		"\0",  // empty string
		"AbC;dE\0",  // ASCII string
		"\xE2\x82\xAC\0",  // A single valid UTF-8
		"Ab\xE2\x82\xAC;dE\0",  // Valid UTF-8 in context
		"Ab\xFF;dE\0",  // One illegal byte
		"Ab\xE2;dE\0",  // One invalid start byte
		"Ab\xE2\xE2;dE\0",  // Two invalid start bytes
		"Ab\xE2\xE2\xE2;dE\0",  // Three invalid start bytes
		"Ab\xE2\xE2...\xE2;dE\0",  // Two disjoint invalid sequences
		"Ab\xE2\x82\xFF;dE\0",  // First two bytes are OK but not the third
		"Ab\xE2\x82\xFF\xE2;dE\0",  // Like above but with another start byte
		"\xE2\0",  // A start byte that "overhangs" the end
		"A\xFD\0",  // Normal ASCII character with invalid byte at end
	};
	const char outputs[][30] = {
		"\"\"\0",
		"\"AbC;dE\"\0",
		"\"\xE2\x82\xAC\"\0",
		"\"Ab\xE2\x82\xAC;dE\"\0",
		"\"Ab\xEF\xBF\xBD;dE\"\0",
		"\"Ab\xEF\xBF\xBD;dE\"\0",
		"\"Ab\xEF\xBF\xBD\xEF\xBF\xBD;dE\"\0",
		"\"Ab\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD;dE\"\0",
		"\"Ab\xEF\xBF\xBD\xEF\xBF\xBD...\xEF\xBF\xBD;dE\"\0",
		"\"Ab\xEF\xBF\xBD\xEF\xBF\xBD;dE\"\0",
		"\"Ab\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD;dE\"\0",
		"\"\xEF\xBF\xBD\"",
		"\"A\xEF\xBF\xBD\"",
	};
	const size_t num_cases = 13;

	int errcode = 0;

	for (size_t i=0; i<num_cases; i++) {
		const char* in = inputs[i];
		const char* expected = outputs[i];
		const size_t expected_len = strlen(expected);

		json_object* strobj = json_object_new_string(in);
		const char* actual = json_object_to_json_string(strobj);
		size_t actual_len = strlen(actual);

		if (expected_len != actual_len) {
			printf("FAIL ON CASE %d: expected length %d but got %d\n",
				(int)i, (int)expected_len, (int)actual_len);
			printf("%s\n", actual);
			errcode = 1;
			goto cleanup;
		}

		if (memcmp(expected, actual, actual_len) != 0) {
			printf("FAIL ON CASE %d: expected '%s' but got '%s'\n",
				(int)i, expected, actual);
			errcode = 2;
			goto cleanup;
		}

		printf("PASS CASE %d\n", (int)i);

cleanup:
		json_object_put(strobj);
	}

	return errcode;
}
