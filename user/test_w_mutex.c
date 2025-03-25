#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

void print_args(int pid, int argc, char *argv[], int mutex_fd) {
	char output[2];
	output[1] = '\0';
	for (int i = 1; i < argc; i++) {
		for (int j = 0; argv[i][j] != '\0'; j++) {
			if(mutex_lock(mutex_fd) < 0) {
				fprintf(2, "Ошибка мьютекса\n");
				exit(1);
			}
			output[0] = argv[i][j];
			printf("pid: %d, arg %d, char '%s'\n", pid, i, output);	
			if(mutex_unlock(mutex_fd) < 0) {
				fprintf(2, "Ошибка мьютекса\n");
				exit(1);
			}
		}
	}
}

int main(int argc, char *argv[]) {
    int mutex_fd = mutex_create();

    if (mutex_fd < 0) {
        fprintf(2, "Ошибка создания мьютекса\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Ошибка fork()\n");
        exit(1);
    }

    if (pid == 0) {
        print_args(getpid(), argc, argv, mutex_fd);
        exit(0);
    } else {
        print_args(getpid(), argc, argv, mutex_fd);
        wait(0);
    }

    exit(0);
}
