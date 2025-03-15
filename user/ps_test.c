#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
	struct procinfo small_buf[1];
	int ret;
	
	ret = ps_listinfo(small_buf, 2);
	if(ret == -1)
		printf("тест для недостаточного размера буфера выполнен\n");
	else if(ret == -2)
		printf("ошибка\n");
	else
		printf("тест для недостаточного размера буфера провален\n");

	ret = ps_listinfo((struct procinfo *)10, 10);
	if(ret == -2)
		printf("тест для неверного адреса выполнен успешно\n");
	else
		printf("тест для неверного адреса провален\n");

	struct procinfo buf[NPROC];
	ret = ps_listinfo(buf, NPROC + 1);
	if(ret <= NPROC && ret > 0)
		printf("тест для корректных данных выполнен, кол-во процессов = %d\n", ret);
	else
		printf("тест для корректных данных провален\n");
	
	exit(0);
}

