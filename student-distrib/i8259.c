/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    

    OUTB(ICW1, MASTER_8259_COMMAND_PORT); /*starts the initialization of the master in cascade mode*/
    OUTB(ICW2_MASTER, MASTER_8259_DATA_PORT); /*IRQ:0-7 mapped to ports x20-27*/
    OUTB(ICW3_MASTER, MASTER_8259_DATA_PORT); /*tells the master PIC that the slave is connected at IRQ2*/
    OUTB(ICW4, MASTER_8259_DATA_PORT);

    OUTB(ICW1, SLAVE_8259_COMMAND_PORT); /* starts the initialization of the slave in cascade mode*/
    OUTB(ICW2_SLAVE, SLAVE_8259_DATA_PORT); /*IRQ:8-15 mapped tp x28-2f*/
    OUTB(ICW3_SLAVE, SLAVE_8259_DATA_PORT); /*tells slave that it is connected to master*/
    OUTB(ICW4, SLAVE_8259_DATA_PORT);

    OUTB(0xFF, MASTER_8259_DATA_PORT); /* mask all of the master and slave ports*/
    OUTB(0xFF, SLAVE_8259_DATA_PORT);

    master_mask = slave_mask = 0xFF; //clears slave and master mask

    enable_irq(2); // enables slave pic irq


}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    // out of bounds checks (0-15)
    if(irq_num > 15 || irq_num < 0) {
        return;
    }
    
    /*enable slave irq*/
    if(irq_num >= 8) {
        irq_num -= 8;
        slave_mask = slave_mask & ~ (1 << irq_num); 
        OUTB(slave_mask, SLAVE_8259_DATA_PORT);
    }
    /*enable mater irq*/
    else {
        master_mask = master_mask & ~ (1 << irq_num);
        OUTB(master_mask, MASTER_8259_DATA_PORT);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    // out of bounds checks (0-15)
    if(irq_num > 15 || irq_num < 0) {
        return;
    }

    /*disable slave_irq*/
    if(irq_num >= 8) {
        irq_num -= 8;
        slave_mask = slave_mask | (1<< irq_num);
        OUTB(slave_mask, SLAVE_8259_DATA_PORT);
    }
    /*disable master_irq*/
    else {
        master_mask = master_mask | (1 << irq_num);
        OUTB(master_mask, MASTER_8259_DATA_PORT);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // out of bounds checks (0-15)
    if(irq_num > 15 || irq_num < 0) {
        return;
    }
    /*send eoi to slave*/
    if(irq_num >= 8) {
        OUTB(EOI | (irq_num-8), SLAVE_8259_COMMAND_PORT);
        OUTB (EOI | (0x02), MASTER_8259_COMMAND_PORT)
    }
    /*send eoi to master*/
    else {
        OUTB(EOI | irq_num, MASTER_8259_COMMAND_PORT); 
    }
    return;
} 
