
#define ASM 1

.text

.global goto_user

goto_user:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%esp), %eax 
    pushl %eax  #pushing SS
    movl 12(%esp), %ebx
    pushl %ebx  #pushing esp
    pushfd
    popl %ecx   #getting 32-bit eflags into ecx
    movl 16(%esp), %edx
    orl %edx, %ecx #bit masking to allow interrupts
    pushl %ecx  #pushing eflags
    movl 20(%esp), %edi
    pushl %edi  #pushing CS
    movl 24(%esp), %esi
    pushl %edi  #pushing eip
    iret

