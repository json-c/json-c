#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "json.h"

#define I32_MAX_S "2147483647"
#define I32_MIN_S "-2147483648"
#define I64_MAX_S "9223372036854775807"
#define I64_OVER  "9223372036854775808"
#define I64_MIN_S "-9223372036854775808"
#define I64_UNDER "-9223372036854775809"
#define U64_MAX_S "18446744073709551615"
#define U64_OUT_S "18446744073709551616"

#define CHECK_GET(GET_F, J, EXPECTED)     { struct json_object *jtmp = J; errno = 0; assert(GET_F(jtmp) == EXPECTED); json_object_put(jtmp); }
#define CHECK_GET_INT(J, EXPECTED)        CHECK_GET(json_object_get_int,    J, EXPECTED)
#define CHECK_GET_INT64(J, EXPECTED)      CHECK_GET(json_object_get_int64,  J, EXPECTED)
#define CHECK_GET_UINT64(J, EXPECTED)     CHECK_GET(json_object_get_uint64, J, EXPECTED)

#define CHECK_BASE(J, EXPECTED)						CHECK_GET_INT(J, EXPECTED); CHECK_GET_INT64(J, EXPECTED); CHECK_GET_UINT64(J, EXPECTED)

#define N_INT     json_object_new_int
#define N_I64     json_object_new_int64
#define N_U64     json_object_new_uint64
#define N_STR     json_object_new_string
#define N_DBL     json_object_new_double

int main(int argc, char **argv)
{
	CHECK_BASE(N_INT(5), 5);
	CHECK_BASE(N_INT(0), 0);
	CHECK_BASE(N_STR("0"), 0);
	CHECK_BASE(N_STR("00000"), 0);
	CHECK_BASE(N_STR("000004568789"), 4568789);
	CHECK_BASE(N_STR("0xFF"), 0 && errno == 0); // Hex-string values being parsed as 0 is the intended behavior
	CHECK_BASE(N_STR("333this_seems_a_valid_string"), 333);
	CHECK_BASE(N_STR("this_is_not_a_number"), 0 && errno == EINVAL);
	CHECK_BASE(N_STR("B0"), 0 && errno == EINVAL);
	printf("BASE CHECK PASSED\n");

	CHECK_GET_INT(N_I64(INT32_MAX), INT32_MAX && errno == 0);
	CHECK_GET_INT(N_I64(INT32_MIN), INT32_MIN && errno == 0);
	CHECK_GET_INT(N_I64(INT64_MAX), INT32_MAX && errno == ERANGE);
	CHECK_GET_INT(N_I64(INT64_MIN), INT32_MIN && errno == ERANGE);
	CHECK_GET_INT(N_STR(I32_MAX_S), INT32_MAX && errno == 0);
	CHECK_GET_INT(N_STR(I32_MIN_S), INT32_MIN && errno == 0);
	CHECK_GET_INT(N_STR(I64_MAX_S), INT32_MAX && errno == ERANGE);
	CHECK_GET_INT(N_STR(I64_MIN_S), INT32_MIN && errno == ERANGE);
	CHECK_GET_INT(N_DBL(INFINITY),  INT32_MAX && errno == ERANGE);
	CHECK_GET_INT(N_DBL(-INFINITY), INT32_MIN && errno == ERANGE);
	CHECK_GET_INT(N_DBL(NAN),       INT32_MIN && errno == EINVAL);
	printf("INT GET PASSED\n");

	CHECK_GET_INT64(N_I64(INT64_MAX), INT64_MAX && errno == 0);
	CHECK_GET_INT64(N_I64(INT64_MIN), INT64_MIN && errno == 0);
	CHECK_GET_INT64(N_STR(I64_MAX_S), INT64_MAX && errno == 0);
	CHECK_GET_INT64(N_STR(I64_MIN_S), INT64_MIN && errno == 0);
	CHECK_GET_INT64(N_STR(I64_OVER),  INT64_MAX && errno == ERANGE);
	CHECK_GET_INT64(N_STR(I64_UNDER), INT64_MIN && errno == ERANGE);
	CHECK_GET_INT64(N_DBL(INFINITY),  INT64_MAX && errno == ERANGE);
	CHECK_GET_INT64(N_DBL(-INFINITY), INT64_MIN && errno == ERANGE);
	CHECK_GET_INT64(N_DBL(NAN),       INT64_MIN && errno == EINVAL);
	printf("INT64 GET PASSED\n");

	CHECK_GET_UINT64(N_U64(UINT64_MAX), UINT64_MAX && errno == 0);
	CHECK_GET_UINT64(N_U64(-1),         UINT64_MAX && errno == 0);
	CHECK_GET_UINT64(N_STR(U64_OUT_S),  UINT64_MAX && errno == ERANGE);
	CHECK_GET_UINT64(N_DBL(INFINITY),   UINT64_MAX && errno == ERANGE);
	CHECK_GET_UINT64(N_DBL(-INFINITY),  0          && errno == ERANGE);
	CHECK_GET_UINT64(N_DBL(NAN),        0          && errno == EINVAL);
	printf("UINT64 GET PASSED\n");

	printf("PASSED\n");
	return 0;
}
