#include <stdint.h>

#include <stddef.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  int *array = new int[100];
  delete [] array;
  return array[size];  // BOOM
}
