#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
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
  uint64 va; // 虚拟地址
  int pagenum; // 页数
  uint64 abitsaddr; // 存放访问位结果的地址

  argaddr(0, &va); // 获取第一个参数：虚拟地址
  argint(1, &pagenum); // 获取第二个参数：页数
  argaddr(2, &abitsaddr); // 获取第三个参数：访问位结果地址

  uint64 maskbits = 0; // 掩码位初始化为0
  struct proc *proc = myproc(); // 获取当前进程结构
  for (int i = 0; i < pagenum; i++) { // 遍历每一页
    pte_t *pte = walk(proc->pagetable, va+i*PGSIZE, 0); // 获取页表条目
    if (pte == 0)
      panic("page not exist."); // 如果页表条目不存在，触发panic
    if (PTE_FLAGS(*pte) & PTE_A) { // 如果页表条目访问位被设置
      maskbits = maskbits | (1L << i); // 设置掩码位的相应位置
    }
    // 清除访问位，将PTE_A位设为0
    *pte = ((*pte & PTE_A) ^ *pte) ^ 0;
  }
  if (copyout(proc->pagetable, abitsaddr, (char *)&maskbits, sizeof(maskbits)) < 0)
    panic("sys_pgaccess copyout error"); // 如果copyout失败，触发panic

  return 0; // 返回0表示成功
}

#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
