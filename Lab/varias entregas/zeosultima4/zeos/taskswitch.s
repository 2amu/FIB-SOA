# 0 "taskswitch.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "taskswitch.S"
# 1 "include/asm.h" 1
# 2 "taskswitch.S" 2
# 1 "include/segment.h" 1
# 3 "taskswitch.S" 2

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
   push %ebp
   mov %esp, %ebp
   pushl %edi
   pushl %esi
   pushl %ebx
   pushl 8(%ebp)

   call inner_task_switch

   addl $4, %ebp

   popl %ebx
   popl %edi
   popl %esi

   pop %ebp
   ret

.globl asinner; .type asinner, @function; .align 0; asinner:
   mov 4(%esp), %eax
   mov %ebp, (%eax)
   mov %esp, 8(%esp)
   pop %ebp
   ret
