#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define A 1
#define D 2
#define AD 3

int global_var = 228;

int main() {
    printf("Таблица страниц на старте\n");
    show_pages(0, 0, AD);

    global_var += 10;
    printf("\n\nТаблица после обращения к глобальной переменной\n");
    show_pages(0, 0, AD);
    
    int local_var = 1337;
    local_var++;
    printf("\n\nТаблица после создания локальной переменной\n");
    show_pages(0, 0, AD);

    int local_arr[1020];
    printf("\n\nТаблица после создания массива на стеке\n");
    show_pages(0, 0, AD);

    for (int i = 0; i < 1020; i += 10)
        local_arr[i] = 42;
    printf("\n\nТаблица после заполнения ячеек массива\n");
    show_pages(0, 0, AD);

    char *heap_arr = sbrk(2 * 4096);
    if ((uint64)heap_arr == -1) {
        fprintf(2, "ошибка malloc\n");
        exit(1);
    }
    printf("\n\nТаблица после создания массива в куче\n");
    show_pages(0, 0, AD);

    memset(heap_arr, 0, 2 * 4096);
    printf("\n\nЗаполним массив в куче нулями\n");
    show_pages((uint64 *)(heap_arr), 2 * 4096, AD);

    printf("\n\nСнимем флаги А\n");
    delete_flags(0, 0, A);
    show_pages(0, 0, AD);

    printf("\n\nСнимем флаги A и D\n");
    delete_flags(0, 0, AD);
    show_pages(0, 0, AD);

    int x = heap_arr[0] + heap_arr[1] + heap_arr[4800];
    printf("%d\n", x);
    printf("\n\nПрочитали данные из массива, должен появиться флаг А\n");
    show_pages((uint64 *)(heap_arr), 2 * 4096, A);

    for (int i = 0; i < 5000; i += 1024) {
        heap_arr[i] = i;
    };
    printf("\n\nЗаписали данные в массив, должен появиться флаг D\n");
    show_pages((uint64 *)(heap_arr), 2 * 4096, D);

    printf("\n\nОсвободим память\n");
    sbrk(-2 * 4096); 
    show_pages(0, 0, AD);

    local_var++;
    local_arr[0]++;
    x++;
    
    exit(0);
}