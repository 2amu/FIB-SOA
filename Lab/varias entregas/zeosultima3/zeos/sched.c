/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

//if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
//#endif

struct list_head freequeue;
struct list_head readyqueue;
struct task_struct *idle_task;

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void asinner();

void task_switch(union task_union *new);

void inner_task_switch(union task_union *new) 
{
	tss.esp0 = &((union task_union*)new)->stack[1024];
	writeMSR(0X175,tss.esp0);
	set_cr3(new->task.dir_pages_baseAddr);
	asinner(&current()->kernel_esp, new->task.kernel_esp);
}

void init_idle (void)
{
	struct list_head *newlist = list_first(&freequeue);
	list_del(newlist);
	struct task_struct *idleproc = list_head_to_task_struct(newlist);
	idleproc->PID = 0;
	allocate_DIR(idleproc);
	((union task_union*)idleproc)->stack[1023] = (unsigned long)&cpu_idle;
	((union task_union*)idleproc)->stack[1022] = 0;
	idleproc->kernel_esp = &((union task_union*)idleproc)->stack[1022];
	idle_task = idleproc;
	
}

void init_task1(void)
{
	struct list_head *newlist = list_first(&freequeue);
	list_del(newlist);
	struct task_struct *task1 = list_head_to_task_struct(newlist);
	task1->PID = 1;
	allocate_DIR(task1);
	set_user_pages(task1);
	tss.esp0 = &((union task_union*)task1)->stack[1024];
	writeMSR(0X175,tss.esp0);
	set_cr3(task1->dir_pages_baseAddr);
}


void init_sched()
{
	INIT_LIST_HEAD(&freequeue);
	for(int i; i < NR_TASKS; ++i) {
		list_add_tail(&task[i].task.list, &freequeue);
	}
	INIT_LIST_HEAD(&readyqueue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

