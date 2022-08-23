#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "processinfo.h"

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
  myproc()->sz=n+myproc()->sz;
 // if(growproc(n) < 0)
   // return -1;
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
  printf("Sleeping for %d ticks\n",n);
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

uint64
sys_echo_simple(void)
{
  char* str="";
  if(argstr(0,str,1000)<0){
	return -1;
  }
  printf("%s \n",str);
  return 0;
}

uint64
sys_echo_kernel(void)
{
   int n;
   char* argv[1000];
   uint64 uargv,uarg;
   if(argint(0,&n)<0||argaddr(1,&uargv)<0){
       return -1;
   }
   for(int i=1;i<n;i++){
       if(fetchaddr(uargv+sizeof(uint64)*i, (uint64*)&uarg) < 0){
	       return -1;
       }
       argv[i]="\0";
       if(fetchstr(uarg,argv[i],4096)<0){
	     return -1;
       }
       printf("%s ",argv[i]);
   }
   printf("\n");
   return 0;
}

uint64
sys_trace(void){
   struct proc* p=myproc();
   int n;
   if(argint(0,&n)<0){
      return -1;
   }
   p->mask=n;

   return 0;
}

uint64
sys_get_process_info(void){
    struct processinfo in;
    struct proc *p = myproc();
    uint64 add;
    if(argaddr(0,&add)<0)
    {	    return -1;}
    in.pid=p->pid;
    in.sz=p->sz;
    strncpy(in.name,p->name,16);
    
    if(copyout(p->pagetable,add,(char*)&in,sizeof(in))<0){
       return -1;
    }
    return 0;
}

