/* idt.h - Defines exceptions and interrupts in IDT
 * vim:ts=4 noexpandtab
 */

#ifndef IDT_H
#define IDT_H

#ifndef ASM
void initialize_idt(void);
#endif /* ASM */

#endif /* IDT_H */
