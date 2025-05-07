#include <asm.h>
#include <segment.h>

ENTRY(write)
  pushl %ebp
  mov %esp, %ebp
  movl $4, %eax
  pushl %edx
  pushl %ecx
  mov 8(%ebp), %edx
  mov 12(%ebp), %ecx
  mov 16(%ebp), %ebx
  
  pushl $returnwrite
  pushl %ebp
  mov %esp, %ebp
  sysenter

returnwrite:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx
  cmpl $0, %eax
  jge finwrite
  negl %eax
  mov %eax, errno
  movl $-1, %eax
  
finwrite:
  mov %ebp, %esp
  popl %ebp
  ret
  
ENTRY(gettime)
  pushl %ebp
  mov %esp, %ebp
  movl $10, %eax
  pushl %edx
  pushl %ecx
  
  push $returngettime
  pushl %ebp
  mov %esp, %ebp
  sysenter
  
returngettime:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx
  
  cmpl $0, %eax
  jge fintime
  negl %eax
  mov %eax, errno
  movl $-1, %eax
  
fintime:
  mov %ebp, %esp
  popl %ebp
  ret
