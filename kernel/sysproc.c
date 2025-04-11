#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
   // lab pgtbl: your code here.
    int pageNum; // number of pages to check
    uint64 addr; // starting virtual addr of first user page to check
    uint64 bitmask; // a pointer to datastructure to store result

    unsigned int bbits = 0; // buffer bits store the res and copy to userspace 
   
    if(argaddr(0, &addr) < 0) return -1; 
    if(argint(1, &pageNum) < 0) return -1;
    if(argaddr(2, &bitmask) < 0) return -1;
   
   
    printf("pageNum: %d\n", pageNum);
    printf("bitmask: 0x%p\n", (void*)bitmask);
    struct proc *p = myproc();  // get current process state info
    for(int i = 0; i < pageNum; i++){
        pte_t *pte = walk(p->pagetable, addr + i * PGSIZE, 0);
        if(pte && (*pte & PTE_V)){
            if(*pte & PTE_A){
                int mbits = (1U << i);
		//printf("mbits: 0x%x\n", mbits);
		bbits |= mbits;
                // printf("bbits: 0x%x\n", bbits);
                *pte = *pte & (~PTE_A); // clear PTE_A after checking 
            }
        }
    }
    // copy buffer bits to bitmask 
    printf("[kernel] bitmask user addr: 0x%p\n", bitmask);
    if(copyout(p->pagetable, bitmask, (char *)&bbits, sizeof(bbits)) < 0 )
        return -1;

    return 0;
	
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


