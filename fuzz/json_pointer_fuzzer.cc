#include <fuzzer/FuzzedDataProvider.h>

#include "json.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	FuzzedDataProvider fdp(data, size);

	struct json_tokener *tokener =
	    json_tokener_new_ex(fdp.ConsumeIntegralInRange<int>(1, JSON_TOKENER_DEFAULT_DEPTH));
	int flags = 0;
	if (fdp.ConsumeBool()) {
		flags |= JSON_TOKENER_VALIDATE_UTF8;
	}
	if (fdp.ConsumeBool()) {
		flags |= JSON_TOKENER_ALLOW_TRAILING_CHARS;
	}
	if (fdp.ConsumeBool()) {
		flags |= JSON_TOKENER_STRICT;
	}
	json_tokener_set_flags(tokener, flags);

	std::string path = fdp.ConsumeRandomLengthString(5);
	std::string sub_json_str = fdp.ConsumeRandomLengthString(10);
	bool use_format_string = fdp.ConsumeBool();
	std::string json_str = fdp.ConsumeRemainingBytesAsString();

	struct json_object *jo1 = json_tokener_parse_ex(tokener, json_str.c_str(), json_str.size());

	struct json_object *sub_json = json_tokener_parse(sub_json_str.c_str());
	if (sub_json == NULL) {
		sub_json = json_object_new_object();
	}

	struct json_object *jo2 = NULL;
	if (use_format_string) {
		json_pointer_getf(jo1, &jo2, "%s", path.c_str());
		if (json_pointer_setf(&jo1, sub_json, "%s", path.c_str()))
		{
			json_object_put(sub_json);
		}
	} else {
		json_pointer_get(jo1, path.c_str(), &jo2);
		if (json_pointer_set(&jo1, path.c_str(), sub_json))
		{
			json_object_put(sub_json);
		}
	}

	// Clean up the main JSON object
	json_object_put(jo1);
	json_tokener_free(tokener);
	return 0;
}
