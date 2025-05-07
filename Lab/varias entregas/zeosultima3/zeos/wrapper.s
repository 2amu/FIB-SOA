# 0 "wrapper.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "wrapper.S"
# 1 "include/asm.h" 1
# 2 "wrapper.S" 2
# 1 "include/segment.h" 1
# 3 "wrapper.S" 2

.globl write; .type write, @function; .align 0; write:
  pushl %ebp
  mov %esp, %ebp
  movl $4, %eax
  pushl %edx
  pushl %ecx
  pushl %ebx
  mov 8(%ebp), %edx
  mov 12(%ebp), %ecx
  mov 16(%ebp), %ebx

  pushl %edx
  pushl %ecx
  push $returnwrite
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
  popl %ebx
  popl %ecx
  popl %edx
  mov %ebp, %esp
  popl %ebp
  ret

.globl gettime; .type gettime, @function; .align 0; gettime:
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


  .globl getpid; .type getpid, @function; .align 0; getpid:
  pushl %ebp
  mov %esp, %ebp
  movl $20, %eax
  pushl %edx
  pushl %ecx

  push $returngetpid
  pushl %ebp
  mov %esp, %ebp
  sysenter

returngetpid:
  popl %ebp
  addl $4, %esp
  popl %ecx
  popl %edx

  cmpl $0, %eax
  jge finpid
  negl %eax
  mov %eax, errno
  movl $-1, %eax

finpid:
  mov %ebp, %esp
  popl %ebp
  ret
