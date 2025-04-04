#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "proc.h"
#include "file.h"
#include <stddef.h>

int mutexalloc(struct file **f) {
	printf("Вызвана функция mutexalloc\n");
	*f = 0;
	struct mutex *m;
	
	*f = filealloc();
	if(!(*f))
		return -1;
	printf("Файл аллоцирован: %p\n", *f);
	
	m = (struct mutex *)kalloc();
	if(!m) {
		printf("Ошибка аллоцирования мьютекса\n");
		fileclose(*f);
		return -1;
	}
	printf("Мьютекс успешно аллоцирован: %p\n", m);

	initsleeplock(&m->sl_lock, "sl_lock");

	(*f)->type = FD_MUTEX;
	(*f)->mutex = m;
	(*f)->readable = 0;
	(*f)->writable = 0;
	
	printf("Мьютекс успешно создан: file = %p, mutex = %p\n", *f, m);
	return 0;
}

int mutexclose(struct file *f) {
	printf("Вызвана функция mutexclose для file = %p\n", f);
	
	if(!f || !f->mutex || f->type != FD_MUTEX){
		printf("Ошибка: кажется это не мьютекс\n");
		return -1;
	}
	
	printf("Освобождение мьютекса: %p\n", f->mutex);
	kfree((char *)f->mutex);
	f->mutex = NULL;
	printf("Мьютекс файла %p успешно закрыт\n", f);

	return 0;
}
