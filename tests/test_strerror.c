#include "strerror_override.h"
#include "strerror_override_private.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	puts(strerror(10000));
	puts(strerror(999));
	return 0;
}
