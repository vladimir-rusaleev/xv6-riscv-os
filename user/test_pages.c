#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 228;

int main() {
    printf("Таблица страниц на старте с глобальной переменной\n");
    show_pages(0, 0, 3);
    
    int local_var = 1337;
    printf("\n\nТаблица после создания локальной переменной\n");
    show_pages(0, 0, 3);

    int local_arr[5000];
    printf("\n\nТаблица после создания массива на стеке\n");
    show_pages(0, 0, 3);

    local_arr[0] = 15;
    printf("\n\nТаблица после заполнения одной из ячеек массива\n");
    show_pages(0, 0, 3);

    char *heap_arr = sbrk(2 * 4096);
    if ((uint64)heap_arr == -1) {
        fprintf(2, "ошибка malloc\n");
        exit(1);
    }
    printf("\n\nТаблица после создания массива в куче\n");
    show_pages(0, 0, 3);

    memset(heap_arr, 0, 2 * 4096);
    printf("\n\nЗаполним массив в куче нулями\n");
    show_pages(0, 0, 3);

    printf("\n\nСнимем флаги А\n");
    delete_flags(0, 0, 1);
    show_pages(0, 0, 3);

    printf("\n\nСнимем флаги A и D\n");
    delete_flags(0, 0, 3);
    show_pages(0, 0, 3);

    int x = heap_arr[0] + heap_arr[1];
    printf("%d\n", x);
    printf("\n\nПрочитали данные из массива, на одной из страниц, занимаемых массивом, появится флаг А\n");
    show_pages((uint64 *)(heap_arr), 2 * 4096, 1);

    heap_arr[0] = 22;
    printf("\n\nЗаписали данные в массив, на одной из страниц, занимаемых массивом, появится флаг D\n");
    show_pages((uint64 *)(heap_arr), 2 * 4096, 2);

    printf("\n\nОсвободим память\n");
    sbrk(-2 * 4096); 
    show_pages(0, 0, 3);

    local_var++;
    local_arr[0]++;
    x++;
    
    exit(0);
}