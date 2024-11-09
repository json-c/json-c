#include <fuzzer/FuzzedDataProvider.h>

#include "json.h"
#include "json_visit.h"

// Function to test json_c_visit
static int emit_object(json_object *jso, int flags, json_object *parent_jso, const char *jso_key,
                       size_t *jso_index, void *userarg) {
	return JSON_C_VISIT_RETURN_CONTINUE;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	FuzzedDataProvider fdp(data, size);
	json_object *jso = json_tokener_parse(fdp.ConsumeRandomLengthString(20).c_str());

	json_object_get_boolean(jso);
	json_object_get_double(jso);
	json_object_get_int(jso);
	json_object_get_int64(jso);
	json_object_get_uint64(jso);
	json_object_get_string(jso);
	json_object_get_string_len(jso);
	json_object_get_object(jso);
	json_object_get_array(jso);
	json_object_get_type(jso);

	json_c_visit(jso, 0, emit_object, NULL);

	json_object_set_int(jso, fdp.ConsumeIntegral<int>());
	json_object_set_int64(jso, fdp.ConsumeIntegral<int64_t>());
	json_object_set_uint64(jso, fdp.ConsumeIntegral<uint64_t>());
	json_object_set_double(jso, fdp.ConsumeFloatingPoint<double>());
	json_object_set_string(jso, fdp.ConsumeRandomLengthString(10).c_str());
	json_object_set_boolean(jso, fdp.ConsumeBool());
	std::string str = fdp.ConsumeRandomLengthString(10);
	json_object_set_string_len(jso, str.c_str(), str.size());

	json_object *dst = NULL;
	json_object_deep_copy(jso, &dst, json_c_shallow_copy_default);
	json_object_put(dst);

	json_object_put(jso);
	return 0;
}
