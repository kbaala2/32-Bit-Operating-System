#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

void div_by_zero_exception(){
    clear();
    printf("divide by zero exception raised");
    while(1);
}
void debug_exception(){
    clear();
    printf("debug exception raised");
    while(1);
}
void NMI_exception(){
    clear();
    printf("non-maskable interrupt exception raised");
    while(1);
}
void breakpoint_exception(){
    clear();
    printf("breakpoint exception raised");
    while(1);
}
void overflow_exception(){
    clear();
    printf("overflow exception raised");
    while(1);
}
void bound_range_exceeded_exception(){
    clear();
    printf("bound range exceeded exception raised");
    while(1);
}
void invalid_opcode_exception(){
    clear();
    printf("invalid opcode exception raised");
    while(1);
}
void device_not_available_exception(){
    clear();
    printf("device not available exception raised");
    while(1);
}
void double_fault_exception(){
    clear();
    printf("double fault exception raised");
    while(1);
}
void coprocessor_segment_overrun(){
    clear();
    printf("coprocessor segment overrun exception raised");
    while(1);
}
void invalid_TSS_exception(){
    clear();
    printf("invalid TSS exception raised");
    while(1);
}
void segment_not_present_exception(){
    clear();
    printf("segment not present exception raised");
    while(1);
}
void stack_fault_exception(){
    clear();
    printf("stack fault exception raised");
    while(1);
}
void general_protection_exception(){
    clear();
    printf("general protection exception raised");
    while(1);
}
void page_fault_exception(){
    clear();
    printf("page-fault exception raised");
    while(1);
}
void x87_FPU_fp_error(){
    clear();
    printf("x87 FPU floating-point exception raised");
    while(1);
}
void alignment_check_exception(){
    clear();
    printf("alignment check exception raised");
    while(1);
}
void machine_check_exception(){
    clear();
    printf("machine check exception raised");
    while(1);
}
void SIMD_fp_exception(){
    clear();
    printf("SIMD Floating-Point exception raised");
    while(1);
}
void system_call(){
    clear();
    printf("System Call raised");
    while(1);
}
// void kb_interrupt(){

// }
// void rtc_interrupt(){

// }
// void asm_link_kb();
// void asm_link_rtc();

// idt_desc_t idt_init;
// idt_init.present = 0x1;
// idt_init.dpl = 0x0;
// idt_init.size = 0x1;
void initialize_idt(){
    SET_IDT_ENTRY(idt[0], div_by_zero_exception);
    SET_IDT_ENTRY(idt[1], debug_exception);
    SET_IDT_ENTRY(idt[2], NMI_exception);
    SET_IDT_ENTRY(idt[3], breakpoint_exception);
    SET_IDT_ENTRY(idt[4], overflow_exception);
    SET_IDT_ENTRY(idt[5], bound_range_exceeded_exception);
    SET_IDT_ENTRY(idt[6], invalid_opcode_exception);
    SET_IDT_ENTRY(idt[7], device_not_available_exception);
    SET_IDT_ENTRY(idt[8], double_fault_exception);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[10], invalid_TSS_exception);
    SET_IDT_ENTRY(idt[11], segment_not_present_exception);
    SET_IDT_ENTRY(idt[12], stack_fault_exception);
    SET_IDT_ENTRY(idt[13], general_protection_exception);
    SET_IDT_ENTRY(idt[14], page_fault_exception);
    SET_IDT_ENTRY(idt[16], x87_FPU_fp_error);
    SET_IDT_ENTRY(idt[17], alignment_check_exception);
    SET_IDT_ENTRY(idt[18], machine_check_exception);
    SET_IDT_ENTRY(idt[19], SIMD_fp_exception);
    SET_IDT_ENTRY(idt[128], system_call);
}
