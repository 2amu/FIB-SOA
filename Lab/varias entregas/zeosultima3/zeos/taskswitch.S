#include <asm.h>
#include <segment.h>

ENTRY(task_switch)
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
   
ENTRY(asinner)
   mov 4(%esp), %eax //&curr
   mov %ebp, (%eax)
   mov %esp, 8(%esp)	// kernel_esp new = %esp
   pop %ebp
   ret
