#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"

#define LIM 32

int
main(int argc, char *argv[]) {
	struct procinfo buf[LIM];

	int ret = ps_listinfo(buf, LIM);
	
	if(ret == -1) {
		fprintf(2, "недостаточный размер буфера\n");
		exit(1);
	}
	if(ret == -2) {
		fprintf(2, "ошибка при записи в буфер\n");
		exit(1);
	}

	for(int i = 0; i < ret; i++) {
		printf("pid: %d\n", buf[i].pid);
		printf("name: %s\n",  buf[i].proc_name);

		if(buf[i].state == PROCINFO_USED)
			printf("state: used\n");
		else if(buf[i].state == PROCINFO_SLEEPING)
			printf("state: sleeping\n");
		else if(buf[i].state == PROCINFO_RUNNABLE)
			printf("state: runnable\n");
		else if(buf[i].state == PROCINFO_RUNNING)
			printf("state: running\n");
		else
			printf("state: zombie\n");
		//printf("state: %s\n", buf[i].state);
		printf("parent pid: %d\n", buf[i].parent_pid);
		printf("\n");
	}
	printf("Всего процессов: %d\n", ret);
	
	exit(0);
}
