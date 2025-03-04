#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		fprintf(2, "Ошибка при создании pipe");
		exit(1);
	}
	
	int pid = fork();
	if(pid == -1){
		fprintf(2, "Ошибка при создании процесса\n");
		exit(1);
	}
	else if(pid == 0){
		close(pipefd[1]);
		
		close(0);
		dup(pipefd[0]);
		close(pipefd[0]);

		char *args[] = {"/wc", 0};
		if( exec("/wc", args) == -1){
			fprintf(2, "Ошибка вызова wc\n");
			exit(1);
		}

		exit(0);
	}
	else{
		close(pipefd[0]);

        	char buf[BUF_SIZE];
		int i, len, len_to_be_written, ptr = 0;
		for(i = 0; i < argc; ++i){
			len = strlen(argv[i]);
			
			if(ptr + len + 1 >= BUF_SIZE) {
				len_to_be_written = ptr;

				while(len_to_be_written > 0){
					int ret = write(pipefd[1], buf, len_to_be_written);
					if( ret == -1){
						fprintf(2, "Ошибка вывода данных\n");
						close(pipefd[1]);
						exit(1);
					}
					len_to_be_written -= ret;
					ptr += ret;
				}

				ptr = 0;
			}

			if(len > BUF_SIZE){
				int ret = write(pipefd[1], argv[i], len);
                                if( ret == -1){
                                        fprintf(2, "Ошибка вывода данных\n");
					close(pipefd[1]);
                                        exit(1);
                                }
			}
			else{
				memmove(buf + ptr, argv[i], len);
				ptr += len;
			}
			buf[ptr] = '\n';
			ptr++;
		}
		if (ptr > 0) {
			if( write(pipefd[1], buf, ptr) == -1){
				fprintf(2, "Ошибка вывода данных\n");
				close(pipefd[1]);
				exit(1);
			}
        	}

        	if( close(pipefd[1]) == -1){
			fprintf(2, "Ошибка закрытия пайпа\n");
			exit(1);
		}

        	wait(0);
		exit(0);
	}
}
