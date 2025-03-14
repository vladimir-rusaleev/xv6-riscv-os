#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
	int pipefd[2];
	if (pipe(pipefd) < 0) {
		perror("Ошибка при создании pipe\n");
		exit(1);
	}

	int pid = fork();
	if (pid < 0) {
		perror("Ошибка при создании процесса\n");
		exit(1);
	}
	else if (pid == 0) {
		if(close(pipefd[1]) < 0) {
			perror("Ошибка закрытия pipe\n");
			exit(1);
		}

		char read_buf[BUF_SIZE];
		ssize_t len_read, len_written, res;

		while ((len_read = read(pipefd[0], read_buf, BUF_SIZE)) > 0) {
			len_written = 0;

			while (len_written < len_read) {
				res = write(1, read_buf + len_written, len_read - len_written);

				if (res < 0) {
					perror("Ошибка при выводе данных\n");
					close(pipefd[0]);
					exit(1);
				}

				len_written += res;
			}
		}
		if (len_read < 0) {
			perror("Ошибка при чтении из файла\n");
			close(pipefd[0]);
			exit(1);
		}

		if(close(pipefd[0]) < 0){
			perror("Ошибка закрытия pipe\n");
			exit(1);
		}

		exit(0);
	}
	else {
		if(close(pipefd[0]) < 0){
			perror("Ошибка закрытия pipe\n");
			exit(1);
		}

		for(int i = 0; i < argc; ++i){
			char *ptr = argv[i];
			ssize_t len = strlen(argv[i]);
			ssize_t len_written = 0;

			while(len_written < len){
				ssize_t ret = write(pipefd[1], ptr + len_written, len);
				if(ret < 0){
					perror("Ошибка вывода данных\n");
					close(pipefd[1]);
					exit(1);
				}
				len_written += ret;
				len -= ret;
			}
			if (write(pipefd[1], "\n", 1) < 0) {
				perror("Ошибка записи переноса строки\n");
				close(pipefd[1]);
				exit(1);
			}
		}

		if (close(pipefd[1]) < 0) {
			perror("Ошибка закрытия пайпа\n");
			exit(1);
		}

		int ret = wait(0);
		if( ret < 0){
			perror("Ошибка wait\n");
			exit(1);
		}

		exit(0);
	}
}
