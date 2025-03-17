#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"


int
main(int argc, char *argv[]) {
	struct procinfo *buf;
	int ret = -1;
	int lim = 2;
	
	while(ret == - 1) {
		buf = malloc(lim * sizeof(struct procinfo));
		if(!buf) {
			fprintf(2, "Ошибка при выделении памяти\n");
			exit(1);
		}
		ret = ps_listinfo(buf, lim);
		
		if(ret >= 0)
			break;
		if(ret == -2) {
			fprintf(2, "ошибка записи в буфер\n");
			free(buf);
			exit(1);
		}
		
		free(buf);
		lim *= 2;
	}

	for(int i = 0; i < ret; i++) {
		printf("pid: %d\n", buf[i].pid);
		printf("name: %s\n",  buf[i].proc_name);

		if(buf[i].state == PROCINFO_SLEEPING)
			printf("state: sleeping\n");
		else if(buf[i].state == PROCINFO_RUNNABLE)
			printf("state: runnable\n");
		else if(buf[i].state == PROCINFO_RUNNING)
			printf("state: running\n");
		else
			printf("state: zombie\n");

		printf("parent pid: %d\n", buf[i].parent_pid);
		printf("parent name: %s\n", buf[i].parent_name);
		printf("\n");
	}
	printf("Всего процессов: %d\n", ret);
	
	free(buf);
	exit(0);
}
