
#define ASM 1

.text

.global asm_link_kb, asm_link_rtc

asm_link_kb:
    pushal
    call keyboard_handler
    popal
    iret

asm_link_rtc:
    pushal
    call rtc_handler
    popal
    iret
