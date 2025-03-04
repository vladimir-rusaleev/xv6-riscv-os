#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
        int pid = fork();
	if(pid < 0){
                fprintf(2, "Ошибка при создании процесса\n");
                exit(1);
        }
        if(pid == 0){
                sleep(100);
                exit(1);
        }
        else{
		int parent_pid = getpid();
                printf("pid родителя = %d, pid ребенка = %d\n", parent_pid, pid);

		if(kill(pid) < 0){
			fprintf(2, "Ошибка kill\n");
		}

                int status_code;
                wait(&status_code);
                printf("Процесс с идентификатором %d был остановлен с кодом возврата %d\n", pid, status_code);

        	exit(0);
	}
}
