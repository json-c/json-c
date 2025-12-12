#include <fuzzer/FuzzedDataProvider.h>

#include "json.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	FuzzedDataProvider fdp(data, size);
	json_object *my_array = json_object_new_array();
	for (int i = 0; i < 3; ++i) {
		json_object *jso = json_tokener_parse(fdp.ConsumeRandomLengthString(10).c_str());
		if (jso == NULL) {
			continue;
		}
		json_object_array_add(my_array, jso);
	}
	json_object_array_insert_idx(my_array, fdp.ConsumeIntegralInRange<size_t>(0, 10),
	                             json_object_new_int(fdp.ConsumeIntegral<int>()));
	json_object_array_get_idx(my_array, fdp.ConsumeIntegralInRange<size_t>(0, 10));
	json_object_array_put_idx(my_array, fdp.ConsumeIntegralInRange<size_t>(0, 10),
	                          json_object_new_int(fdp.ConsumeIntegral<int>()));
	json_object_array_del_idx(my_array, fdp.ConsumeIntegralInRange<size_t>(0, 10),
	                          fdp.ConsumeIntegralInRange<size_t>(0, 10));
	json_object_array_shrink(my_array, fdp.ConsumeIntegralInRange<size_t>(0, 10));
	json_object_array_sort(my_array, [](const void *a, const void *b) { return 0; });
	json_object_array_length(my_array);
	json_object_put(my_array);
	return 0;
}
