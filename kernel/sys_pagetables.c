#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"


void print_pagetable(pagetable_t pagetable, int level, uint64 buf_st, int len, int flags) {
  static char* prefixes[] = {"...................", "......... ", ""};
  uint64 sizeof_block = 1LL << (9 * level + PGSHIFT);

  if(level > 2 || level < 0)
    return;

  uint64 rounded_buf;
  for(int i = 0; i < 512; i++) {

    if(buf_st != 0 && len <= 0)
        break;
    
    uint64 block_start = i * sizeof_block;
    uint64 block_end = block_start + sizeof_block;

    if(buf_st != 0 && (block_end <= buf_st || block_start >= buf_st + len))
        continue;

    rounded_buf = PGROUNDDOWN(buf_st);
    pte_t *pte;
    if(buf_st == 0)
        pte = &pagetable[i];
    else
        pte = &pagetable[(rounded_buf / sizeof_block) % 512];

    int flag_validation = (((flags == A_FLAG) && (*pte & PTE_A)) || ((flags == D_FLAG) && (*pte & PTE_D)) || (flags == ALL_FLAGS));

    if ((*pte & PTE_V) && (level > 0 || flag_validation)){
        printf("%s", prefixes[level]);
        
        pagetable_t pgt = (pagetable_t)PTE2PA(*pte);

        if (i < 0x010)
            printf("0x00");
        else if (i < 0x100)
            printf("0x0");
        else
            printf("0x");

        printf("%x -> %p ", i, pgt);
        
        char flags_[8] = {
            (*pte & PTE_R) ? 'R' : '_',
            (*pte & PTE_W) ? 'W' : '_',
            (*pte & PTE_X) ? 'X' : '_',
            (*pte & PTE_U) ? 'U' : '_',
            (*pte & PTE_G) ? 'G' : '_',
            (*pte & PTE_A) ? 'A' : '_',
            (*pte & PTE_D) ? 'D' : '_',
            '\0'
        };
        printf("%s\n", flags_);

        print_pagetable(pgt, level - 1, buf_st, len, flags);
    }

    if (*pte & PTE_V) {
        uint64 buf_left = sizeof_block - (buf_st - rounded_buf);

        if(buf_st != 0)
            buf_st = rounded_buf + sizeof_block;

        if(buf_left > len)
            buf_left = len;
        
        len -= buf_left;
    }

    }
  
}

void delete_flags(pagetable_t pagetable, int level, uint64 buf_st, int len, int flags) {
  uint64 sizeof_block = 1LL << (9 * level + PGSHIFT);

  if(level > 2 || level < 0)
    return;

  uint64 rounded_buf;
  for(int i = 0; i < 512; i++) {

    if(buf_st != 0 && len <= 0)
        break;
    
    uint64 block_start = i * sizeof_block;
    uint64 block_end = block_start + sizeof_block;
        
    if(buf_st != 0 && (block_end <= buf_st || block_start >= buf_st + len))
        continue;

    rounded_buf = PGROUNDDOWN(buf_st);
    pte_t *pte;
    if(buf_st == 0)
        pte = &pagetable[i];
    else
        pte = &pagetable[(rounded_buf / sizeof_block) % 512];

    int flag_validation = (((flags == A_FLAG) && (*pte & PTE_A)) || ((flags == D_FLAG) && (*pte & PTE_D)) || (flags == ALL_FLAGS));

    if ((*pte & PTE_V) && (level > 0 || flag_validation)){
        if(flags == A_FLAG)
            *pte &= ~PTE_A;
        else if(flags == D_FLAG)
            *pte &= ~PTE_D;
        else if(flags == ALL_FLAGS) {
            *pte &= ~PTE_A;
            *pte &= ~PTE_D;
        }
        
        pagetable_t pgt = (pagetable_t)PTE2PA(*pte);

        delete_flags(pgt, level - 1, buf_st, len, flags);
    }

    if (*pte & PTE_V) {
        uint64 buf_left = sizeof_block - (buf_st - rounded_buf);

        if(buf_st != 0)
            buf_st = rounded_buf + sizeof_block;

        if(buf_left > len)
            buf_left = len;
        
        len -= buf_left;
    }

    }
  
}

uint64
sys_show_pages(void)
{
  uint64 buf;
  int len, flags;

  argaddr(0, &buf);
  argint(1, &len);
  argint(2, &flags);

  if(flags & ~(D_FLAG | A_FLAG))
    return -1;

  struct proc *p = myproc();
  pagetable_t pagetable = p->pagetable;

  printf("PAGETABLE %p\n", pagetable);

  if(buf == 0 || len == 0)
    print_pagetable(pagetable, 2, 0, 0, flags);
  else
    print_pagetable(pagetable, 2, buf, len, flags);
  
  return 0;
}

uint64
sys_delete_flags(void)
{
  uint64 buf;
  int len, flags;

  argaddr(0, &buf);
  argint(1, &len);
  argint(2, &flags);

  if(flags & ~(D_FLAG | A_FLAG))
    return -1;

  struct proc *p = myproc();
  pagetable_t pagetable = p->pagetable;

  if(buf == 0 || len == 0)
    delete_flags(pagetable, 2, 0, 0, flags);
  else
    delete_flags(pagetable, 2, buf, len, flags);
  
  return 0;
}