#ifdef NDEBUG
#undef NDEBUG
#endif
#include "strerror_override.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	puts(strerror(10000));
	puts(strerror(999));
	/* Negative values must not index before the digit table (out-of-bounds
	 * read); INT_MIN also exercises the unsigned magnitude handling. */
	puts(strerror(-5));
	puts(strerror(-2147483647 - 1));
	return 0;
}
