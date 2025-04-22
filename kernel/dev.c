#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "proc.h"

#define DEV_NULL      0
#define DEV_ZERO      1
#define DEV_URANDOM   2
#define DEV_NULLSTAT  3

struct nullstat_data {
    uint64 count;
    struct spinlock lock;
} nullstat;
  
uint64 rand_seed = 1;
  
static uint64 lcg(uint seed) {
    return (1103515245 * seed + 12345) % 2147483648;
}

int dev_read(short minor, int user_dst, uint64 dst, int n) {

    switch (minor){
        case DEV_NULL: return 0;

        case DEV_ZERO:
            char zero_byte = 0;
            for (int i = 0; i < n; ++i) {
                if (either_copyout(user_dst, dst + i, &zero_byte, 1) < 0)
                    return -1;
            }
            return n;
        
        case DEV_URANDOM:
            for (int i = 0; i < n; ++i) {
                rand_seed = lcg(rand_seed);
                char urand = rand_seed & 0xFF;

                if (either_copyout(user_dst, dst + i, &urand, 1) < 0)
                    return -1;
            }

            return n;

        case DEV_NULLSTAT:
            if (n != sizeof(uint64))
                return -1;
            
            acquire(&nullstat.lock);
            uint64 cnt = nullstat.count;
            release(&nullstat.lock);
            if (either_copyout(user_dst, dst, &cnt, n) < 0) 
                return -1;
            
            return n;
        
        default:
            return -1;
    }   
}

int dev_write(short minor, int user_src, uint64 src, int n) {

    switch (minor) {
        case DEV_NULL: 
            return n;

        case DEV_ZERO:
            return -1;

        case DEV_URANDOM:
            if (n != sizeof(uint64))
                return -1;

            uint64 new_seed;
            if (either_copyin(&new_seed, user_src, src, n) < 0)
                return -1;

            if(new_seed)
                rand_seed = new_seed;
            else
                rand_seed = 1;

            return n;
        
        case DEV_NULLSTAT:
            acquire(&nullstat.lock);
            nullstat.count += n;
            release(&nullstat.lock);

            return n;
        
        default:
            return -1;
    }
}

void dev_init(void) {
    initlock(&nullstat.lock, "dev");
    devsw[DEV].read = dev_read;
    devsw[DEV].write = dev_write;
  }