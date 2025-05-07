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
extern int zeos_ticks;

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

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

int sys_write(int fd, char *buffer, int size) {
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
		copy_from_user(&buffer+wrote, &buffos, size-wrote);
		wrote = wrote + sys_write_console(&buffos, size-wrote);
	}
	return size;
}

int sys_gettime() {
	return zeos_ticks;
}

void sys_exit()
{  
}
