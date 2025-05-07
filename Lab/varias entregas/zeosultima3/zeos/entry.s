# 0 "entry.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 1 "include/errno.h" 1
# 8 "entry.S" 2
# 72 "entry.S"
.globl kb_handler; .type kb_handler, @function; .align 0; kb_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call kb_routine
 movb $0x20, %al ; outb %al, $0x20
 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl clk_handler; .type clk_handler, @function; .align 0; clk_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al ; outb %al, $0x20
 call clk_routine
 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl pgfault_handler; .type pgfault_handler, @function; .align 0; pgfault_handler:
 call pgfault_routine

.globl syscall_handler_sysenter; .type syscall_handler_sysenter, @function; .align 0; syscall_handler_sysenter:
 push $0x2B
 push %ebp
 pushfl
 push $0x23
 push 4(%ebp)
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 cmpl $0, %eax
 jl sysenter_err
 cmpl $MAX_SYSCALL, %eax
 jg sysenter_err
 call *sys_call_table(, %eax, 0x04)
 jmp sysenter_fin
sysenter_err:
 movl $-38, %eax
sysenter_fin:
 movl %eax, 0x18(%esp)
 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 movl (%esp), %edx
 movl 12(%esp), %ecx
 sti
 sysexit

.globl writeMSR; .type writeMSR, @function; .align 0; writeMSR:
  push %ebp
  mov %esp, %ebp
  mov 8(%ebp), %ecx
  mov 12(%ebp), %eax
  xor %edx, %edx
  wrmsr
  mov %ebp, %esp
  popl %ebp
  ret
