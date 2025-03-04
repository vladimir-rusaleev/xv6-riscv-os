#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		fprintf(stderr, "Ошибка при создании pipe\n");
		exit(1);
	}

	int pid = fork();
	if (pid == -1) {
		fprintf(stderr, "Ошибка при создании процесса\n");
		exit(1);
	}
	else if (pid == 0) {
		close(pipefd[1]);

		char read_buf[BUF_SIZE];
		int len_read, len_written, res;

		while ((len_read = read(pipefd[0], read_buf, BUF_SIZE)) > 0) {
			len_written = 0;

			while (len_written < len_read) {
				res = write(1, read_buf + len_written, len_read - len_written);

				if (res < 0) {
					fprintf(stderr, "Ошибка при выводе данных\n");
					close(pipefd[0]);
					exit(1);
				}

				len_written += res;
			}
		}
		if (len_read < 0) {
			fprintf(stderr, "Ошибка при чтении из файла\n");
			close(pipefd[0]);
			exit(1);
		}

		close(pipefd[0]);
		exit(0);
	}
	else {
		close(pipefd[0]);

		char buf[BUF_SIZE];
		int i, len, len_to_be_written, ptr = 0;
		for (i = 0; i < argc; ++i) {
			len = strlen(argv[i]);

			if (ptr + len + 1 >= BUF_SIZE) {
				len_to_be_written = ptr;

				while (len_to_be_written > 0) {
					int ret = write(pipefd[1], buf + ptr, len_to_be_written);
					if (ret == -1) {
						fprintf(stderr, "Ошибка вывода данных\n");
						close(pipefd[1]);
						exit(1);
					}
					len_to_be_written -= ret;
					ptr += ret;
				}

				ptr = 0;
			}

			if (len > BUF_SIZE) {
				int ret = write(pipefd[1], argv[i], len);
				if (ret == -1) {
					fprintf(stderr, "Ошибка вывода данных\n");
					close(pipefd[1]);
					exit(1);
				}
			}
			else {
				memmove(buf + ptr, argv[i], len);
				ptr += len;
			}
			buf[ptr] = '\n';
			ptr++;
		}
		if (ptr > 0) {
			if (write(pipefd[1], buf, ptr) == -1) {
				fprintf(stderr, "Ошибка вывода данных\n");
				close(pipefd[1]);
				exit(1);
			}
		}

		if (close(pipefd[1]) == -1) {
			fprintf(stderr, "Ошибка закрытия пайпа\n");
			close(pipefd[1]);
			exit(1);
		}

		wait(0);
		exit(0);
	}
}
