#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static char hex_alph[] = "0123456789abcdef";
 
int main(int argc, char *argv[]) {
    if( argc != 3) {
        fprintf(2, "Неверное число аргументов\n");
        exit(1);
    }

    int fd = open(argv[1], 0);
    if(fd < 0) {
        fprintf(2, "Ошибка open\n");
        exit(1);
    }
    int n = atoi(argv[2]);

    unsigned char buf[16];
    int total = 0;

    while(total < n) {
        int to_read = (n - total > sizeof(buf)) ? sizeof(buf) : (n - total);
        int res = read(fd, buf, to_read);
        if(res < 0) {
            fprintf(2, "ошибка read\n");
            exit(1);
        }
        
        for(int i = 0; i < res; i++) {
            char wr[3];
            wr[0] = hex_alph[buf[i] >> 4];
            wr[1] = hex_alph[buf[i] & 0xF];
            wr[2] = ' ';

            if(write(1, wr, 3) < 0) {
                fprintf(2, "ошибка write\n");
                exit(1);
            }
        }

        total += res;
    }

    write(1, "\n", 1);

    close(fd);
    exit(0);
}