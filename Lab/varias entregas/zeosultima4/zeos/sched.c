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

int remainingquant = 0;


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

void init_stats(struct stats *st) 
{
  st->user_ticks = 0;
  st->system_ticks = 0;
  st->blocked_ticks = 0;
  st->ready_ticks = 0;
  st->elapsed_total_ticks = get_ticks();
  st->total_trans = 0; /* Number of times the process has got the CPU: READY->RUN transitions */
  st->remaining_ticks = get_ticks();
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

int get_quantum (struct task_struct *t) 
{
	return t->quantum;
}

void set_quantum (struct task_struct *t, int new_quantum)
{
	t->quantum = new_quantum;
}

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
	idleproc->quantum = 15;
	init_stats(&idleproc->statistics);
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
	task1->quantum = 15;
	task1->state = ST_READY;
	remainingquant = task1->quantum;
	init_stats(&task1->statistics);
	allocate_DIR(task1);
	set_user_pages(task1);
	tss.esp0 = &((union task_union*)task1)->stack[1024];
	writeMSR(0X175,tss.esp0);
	set_cr3(task1->dir_pages_baseAddr);
}

void update_sched_data_rr (void) 
{
	remainingquant -= 1;
	current()->statistics.remaining_ticks = remainingquant;
}

int needs_sched_rr (void) 
{
	if (remainingquant == 0) {
		
		if (!list_empty(&readyqueue)) return 1;
		else {
			remainingquant = current()->quantum;
			return 0;
	}
	else return 0;
}

void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue) 
{
	struct list_head *tlist = &t->list;
	if(list->prev && list->next) list_del(tlist);
	
	if (dst_queue != NULL) {
		list_add_tail(t,dst_queue);
		t->state = ST_READY;
		t->statistics.system_ticks += get_ticks() - t->statistics.elapsed_total_ticks;
	}
}

void schedule() 
{
	update_sched_data_rr();
	if (needs_sched_rr()) {
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}

void sched_next_rr (void) 
{
	struct list_head *l;
	struct task_struct *t;
	
	if (!list_empty(&readyqueue)) {
		l = list_first(&readyqueue);
		list_del(l);
		
		t = list_head_to_task_struct(l);
	}	
	else t = idle_task;
	
	t->state = ST_RUN;
	remaingquant = get_quantum(t);
	current()->statistics.system_ticks += get_ticks() - current()->statistics.elapsed_total_ticks;
	current()->statistics.elapsed_total_ticks = get_ticks();
	
	t->statistics.ready_ticks += get_ticks() - t->statistics.elapsed_total_ticks;
	t->statistics.elapsed_total_ticks = get_ticks();
	t->statistics.total_trans += 1;
	task_switch((union task_union*) t);
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

