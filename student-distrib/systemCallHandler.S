
#define ASM 1

.text

.global sys_call_handler

.align 4
sys_call_table:
    .long 0
    .long sys_halt
    .long sys_execute
    .long sys_read
    .long sys_write
    .long sys_open
    .long sys_close
    .long sys_getargs
    .long sys_vidmap
    .long sys_set_handler
    .long sys_sigreturn

.align 4
sys_call_handler:
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    
    pushfl

    pushl %edx
    pushl %ecx
    pushl %ebx

    cmpl $11, %eax
    jae invalid_call
    cmpl $0, %eax
    jng invalid_call
    
    call *sys_call_table(0, %eax, 4)
    jmp sys_call_complete

invalid_call:
    movl $-1, %eax

sys_call_complete:
    addl $12, %esp

    popfl
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp

    iret
