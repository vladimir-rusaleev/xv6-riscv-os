#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int pipefd[2];
	if (pipe(pipefd) < 0) {
		fprintf(2, "Ошибка при создании pipe");
		exit(1);
	}
	
	int pid = fork();
	if(pid < 0){
		fprintf(2, "Ошибка при создании процесса\n");
		exit(1);
	}
	else if(pid == 0){
		if(close(pipefd[1]) < 0) {
			fprintf(2, "Ошибка при закрытии pipe\n");
			exit(1);
		}
		if(close(0) < 0) {
			fprintf(2, "Ошибка при закрытии stdin\n");
			exit(1);
		}
		if(dup(pipefd[0]) < 0) {
			fprintf(2, "Ошибка при вызове dup\n");
			exit(1);
		}
		if(close(pipefd[0]) < 0) {
			fprintf(2, "Ошибка при закрытии pipe\n");
			exit(1);
		}

		char *args[] = {"/wc", 0};
		if( exec("/wc", args) < 0){
			fprintf(2, "Ошибка вызова wc\n");
			exit(1);
		}
		fprintf(2, "Ошибка exec\n");
		exit(1);
	}
	else{
		if(close(pipefd[0]) < 0){
			fprintf(2, "Ошибка закрытия pipe\n");
			exit(1);
		}

		for(int i = 0; i < argc; ++i){
			char *ptr = argv[i];
			int len = strlen(argv[i]);
			int len_written = 0;

			while(len_written < len){
				int ret = write(pipefd[1], ptr + len_written, len);
				if(ret < 0){
					fprintf(2, "Ошибка вывода данных\n");
					close(pipefd[1]);
					exit(1);
				}
				len_written += ret;
				len -= ret;
			}
			if (write(pipefd[1], "\n", 1) < 0) {
                		fprintf(2, "Ошибка записи переноса строки\n");
                		close(pipefd[1]);
                		exit(1);
            		}
		}

		if( close(pipefd[1]) < 0){
			fprintf(2, "Ошибка закрытия пайпа\n");
			exit(1);
		}

		int ret = wait(0);
		if(ret < 0){
			fprintf(2, "Ошибка wait\n");
			exit(1);
		}
		exit(0);
	}
}
