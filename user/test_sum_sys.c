#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE 70

int
main(int argc, char *argv[])
{
	int x = 11, y = 22;
	printf("%d + %d = %d\n", x, y, add(x, y));
	exit(0);
}

