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
	printf("Файл аллоцирован\n");
	
	m = (struct mutex *)kalloc();
	if(!m) {
		fileclose(*f);
		return -1;
	}
	m->sl_lock = (struct sleeplock *)kalloc();
	if(!(m->sl_lock)) {
		kfree((char *)m);
		fileclose(*f);
		return -1;
	}

	initsleeplock(m->sl_lock, "sl_lock");

	(*f)->type = FD_MUTEX;
	(*f)->mutex = m;
	(*f)->readable = 0;
	(*f)->writable = 0;
	
	printf("Мьютекс успешно создан\n");
	return 0;
}

int mutexclose(struct file *f) {
	printf("Вызвана функция mutexclose\n");
	
	if(!f || !f->mutex || f->type != FD_MUTEX){
		printf("Ошибка: кажется это не мьютекс\n");
		return -1;
	}
	if(holdingsleep(f->mutex->sl_lock) && f->mutex->sl_lock->pid == myproc()->pid)
		releasesleep(f->mutex->sl_lock);

	kfree((char *)f->mutex->sl_lock);
	kfree((char *)f->mutex);
	f->mutex = NULL;
	printf("Мьютекс успешно закрыт\n");
	

	return 0;
}
