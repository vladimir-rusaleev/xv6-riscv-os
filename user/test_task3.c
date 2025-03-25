#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUF_SIZE 16

int main(int argc, char *argv[]) {
	int mutex_fd, pid;
	
	mutex_fd = mutex_create();
	if( mutex_fd < 0) {
		fprintf(2, "Ошибка сохдания мьютекса\n");
		exit(1);
	}

	char buf[BUF_SIZE];
	int ret;

	if((ret = read(mutex_fd, buf, sizeof(buf))) < 0)
		printf("Тест на чтение пройден успешно\n");
	else
		printf("Тест на чтение провален\n");

	if((ret = write(mutex_fd, buf, sizeof(buf))) < 0)
		printf("Тест на запись пройден успешно\n");
	else
		printf("Тест на запись провален\n");
	if(close(mutex_fd) < 0){
		fprintf(2, "Ошибка закрытия мьютекса\n");
		exit(1);
	}

	printf("\n");
	
	mutex_fd = mutex_create();
	if(mutex_fd < 0) {
		fprintf(2, "Ошибка создания мьютекса\n");
		exit(1);
	}
	if(mutex_lock(mutex_fd) < 0){
		fprintf(2, "Ошибка lock\n");
		exit(1);
	}
	if(close(mutex_fd) < 0)
		printf("Закрытие процессом, создавшим мьютекс, провалено\n");
	else
		printf("Закрытие процессом, создавшим мьютекс, прошло успешно\n");

	printf("\n");
	
	mutex_fd = mutex_create();
	if( mutex_fd < 0) {
		fprintf(2, "Ошибка создания мьютекса\n");
		exit(1);
	}
	if( mutex_lock(mutex_fd) < 0) {
		fprintf(2, "Ошибка lock\n");
		exit(1);
	}
	if((pid = fork()) < 0) {
		fprintf(2, "Ошибка fork\n");
		exit(1);
	}
	else if(pid > 0) {
		sleep(10);
		if( mutex_unlock(mutex_fd) < 0) {
			fprintf(2, "Ошибка unlock\n");
			exit(1);
		}
		if( close(mutex_fd) < 0)
			printf("Ошибка при закрытии мьютекса\n");
		else
			printf("Мьютекс уже закрыт дочерним процессом, поэтому в следующей строчке вы \
не должны увидеть сообщение :'Мьютекс успешно закрыт'\n");
		wait(0);
	}
	else {
		if(close(mutex_fd) < 0)
			printf("Ошибка закрытия мьютекса\n");
		else
			printf("Дочерний процесс успешно закрыл мьютекс\n");
		exit(0);
	}
	
	printf("\n");

	if((pid = fork()) < 0) {
		fprintf(2, "Ошибка fork\n");
		exit(1);
	}
	else if(pid == 0) {
		int test3_mutex = mutex_create();
		if(test3_mutex < 0) {
			fprintf(2, "Ошибка создания мьютекса\n");
			exit(1);
		}
		if(mutex_lock(test3_mutex) < 0){
			fprintf(2, "Ошибка lock\n");
			exit(1);
		}
		printf("Выходим из дочернего процесса, не закрыв перед этим мьютекс, \
сейчас он должен автоматически закрыться\n");
		exit(0);
	}
	else {
		wait(0);
	}
	
	printf("\n");
	mutex_fd = mutex_create();
	if(mutex_fd < 0) {
		fprintf(2, "Ошибка при создании мьютекса\n");
		exit(1);
	}
	if(mutex_lock(mutex_fd) < 0) {
		fprintf(2, "Ошибка lock\n");
		exit(1);
	}
	if((pid = fork()) < 0) {
		fprintf(2,"Ошибка fork\n");
		exit(1);
	}
	else if(pid == 0) {
		if(mutex_unlock(mutex_fd) < 0)
			printf("Другой процесс не смог разблокировать мьютекс, все ок\n");
		else
			printf("Другой процесс смог разблокировать мьютекс, тест провален\n");
		exit(0);
	}
	else {
		sleep(10);
		if(mutex_unlock(mutex_fd) < 0) {
			fprintf(2, "Ошибка unlock\n");
			exit(1);
		}
		wait(0);
	}

	exit(0);
}
