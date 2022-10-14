/* idt.h - Defines exceptions and interrupts in IDT
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

void div_by_zero_exception();
void debug_exception();
void NMI_exception();
void breakpoint_exception();
void overflow_exception();
void bound_range_exceeded_exception();
void invalid_opcode_exception();
void device_not_available_exception();
void double_fault_exception();
void coprocessor_segment_overrun();
void invalid_TSS_exception();
void segment_not_present();
void stack_segment_fault();
void general_protection_exception();
void page_fault_exception();
void x87_FPU_fp_error();
void alignment_check_exception();
void machine_check_exception();
void SIMD_fp_exception();
void system_call();

