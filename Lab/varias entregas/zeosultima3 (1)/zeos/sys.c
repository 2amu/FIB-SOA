/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>


#define LECTURA 0
#define ESCRIPTURA 1

char buffos[4096];
int pidcount = 100;
//extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork()
{
	return 0;
}

int sys_fork()
{
  int PID=-1;
  if(list_empty(&freequeue)) return ENOMEM;
  struct list_head *newlist = list_first(&freequeue);
  list_del(newlist);
  struct task_struct *childproc = list_head_to_task_struct(newlist);
  
  copy_data(current(), childproc, sizeof(task_union));
  allocate_DIR(childproc);
  
  unsigned int paginas[NUM_PAG_DATA];
  for(int npag = 0; npag < NUM_PAG_DATA, npag++) {
  	int pag = alloc_frame();
  	if (pag == -1) { // frame ocupado
  		for(int pagocupada = 0; pagocupada<npag; pagocupada++) {
			free_frame(paginas[pagocupada]);  		
  		}
  		list_add_tail(newlist, &freequeue);
  		return ENOMEM;
  		}
  	else paginas[npag] = pag;
  }
  
  page_table_entry *ptchild = get_PT(childproc);
  page_table_entry *ptdad = get_PT(current());
  int maxim = max(NUM_PAG_KERNEL, NUM_PAG_CODE, NUM_PAG_DATA);
  for(int i = 0; i < maxim; i++) {
  	if (i < NUM_PAG_KERNEL) set_ss_pag(ptchild,i,get_frame(ptdad,i));
  	if (i < NUM_PAG_DATA) set_ss_pag(ptchild, PAG_LOG_INIT_DATA + i, paginas[i]);
  	if (i < NUM_PAG_CODE) set_ss_pag(ptchild, PAG_LOG_INIT_CODE + i, get_frame(ptdad, NUM_PAG_KERNEL+NUM_PAG_DATA+i));
  }
  
  int shared = NUM_PAG_KERNEL + NUM_PAG_CODE;
  int total = NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA;
  
  for (int j = shared; j < total; ++j) {
  	set_ss_pag(ptdad, j+NUM_PAG_DATA+NUM_DATA_CODE, get_frame(ptchild,j));
  	copy_data((void *) (j<<12), (void *) ((j+NUM_PAG_DATA) << 12), PAG_SIZE); //???
  	del_ss_pag(ptdad, j + NUM_PAG_DATA);
  }
  
  set_cr3(get_DIR(current());
  
  childproc->PID = ++pidcount;
  
  union task_union *childunion = (union task_union*) childproc;
  childunion->stack[KERNEL_STACK_SIZE-19] = 0; //fake EBP
  childunion->stack[KERNEL_STACK_SIZE-18] = (long unsigned int) &ret_from_fork;
  childproc->kernel_esp = (unsigned int)&(childunion->stack[KERNEL_STACK_SIZE-19])
  
  list_add_tail(&(childunion->task.list), &readyqueue);
  
  PID = childunion->task.PID;
  
  return PID;
}

int sys_write(int fd, char *buffer, int size) 
{
	int id = check_fd(fd, ESCRIPTURA);
	if (id != 0) return id;
	if(buffer == NULL) return EFAULT;
	if(size < 0) return EINVAL;
	access_ok(fd,buffer,size);
	
	int wrote = 0;
	while((size-wrote) > 4096) {
		copy_from_user(&buffer[wrote], &buffos, 4096);
		wrote = wrote + sys_write_console(&buffos[wrote], 4096);
	}
	if (size-wrote != 0) {
		copy_from_user(&buffer[wrote], &buffos, size-wrote);
		wrote = wrote + sys_write_console(&buffos, size-wrote);
	}
	return size;
}

int sys_gettime() 
{
	return zeos_ticks;
}

void sys_exit()
{  
	struct task_struct *exitproc = current();
	page_table_entry *exitPT = get_PT(exitproc);
	for(int i = 0; i<NUM_PAG_DATA; ++i) {
		free_frame(get_frame(exitPT, i+PAG_LOG_INIT_DATA));
		del_ss_pag(exitPT, i+PAG_LOG_INIT_DATA);
	}
	exitproc->PID = -1;
	exitproc->dir_pages_baseAddr = NULL;
	update_process_state_rr(exitproc, &freequeue);
	sched_next_rr();
}
