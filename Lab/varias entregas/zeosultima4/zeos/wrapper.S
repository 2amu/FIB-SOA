#include <asm.h>
#include <segment.h>

ENTRY(write)
  pushl %ebp
  movl %esp, %ebp
  movl $4, %eax
  pushl %edx
  pushl %ecx
  pushl %ebx
  movl 8(%ebp), %edx
  movl 12(%ebp), %ecx
  movl 16(%ebp), %ebx
  
  pushl %edx
  pushl %ecx
  push $returnwrite
  pushl %ebp
  movl %esp, %ebp
  sysenter

returnwrite:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx
  cmpl $0, %eax
  jge finwrite
  negl %eax
  movl %eax, errno
  movl $-1, %eax
  
finwrite:
  popl %ebx
  popl %ecx
  popl %edx
  movl %ebp, %esp
  popl %ebp
  ret
  
ENTRY(gettime)
  pushl %ebp
  movl %esp, %ebp
  movl $10, %eax
  pushl %edx
  pushl %ecx
  
  push $returngettime
  pushl %ebp
  movl %esp, %ebp
  sysenter
  
returngettime:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx
  
  cmpl $0, %eax
  jge fintime
  negl %eax
  movl %eax, errno
  movl $-1, %eax
  
fintime:
  movl %ebp, %esp
  popl %ebp
  ret
  
  
ENTRY(getpid)
  pushl %ebp
  movl %esp, %ebp
  movl $20, %eax
  pushl %edx
  pushl %ecx
  
  push $returngetpid
  pushl %ebp
  movl %esp, %ebp
  sysenter
  
returngetpid:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx
  
  cmpl $0, %eax
  jge finpid
  negl %eax
  movl %eax, errno
  movl $-1, %eax
  
finpid:
  movl %ebp, %esp
  popl %ebp
  ret
  
ENTRY(fork)
  pushl %ebp
  movl %esp, %ebp
  movl $2, %eax
  pushl %edx
  pushl %ecx
  
  push $returnfork
  pushl %ebp
  movl %esp, %ebp
  sysenter
  
returnfork:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx
  
  cmpl $0, %eax
  jge finfork
  negl %eax
  movl %eax, errno
  movl $-1, %eax
  
finfork:
  movl %ebp, %esp
  popl %ebp
  ret
  
  
ENTRY(exit)
  pushl %ebp
  movl %esp, %ebp
  movl $1, %eax
  sysenter
  popl %ebp
  ret
  
