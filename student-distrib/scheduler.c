


#include "scheduler.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"
#include "paging.h"
#include "terminal.h"
#include "sysCalls.h"

#define _8MB 0x800000
#define PCB_SIZE 0x1000

int8_t prog_timer = 0;

int32_t get_next_terminal(int32_t curr){
    int32_t next_terminal = (curr + 1) % MAX_TERMINALS;
    return next_terminal;
}

void pit_init(void){
    //set the frequency of the PIT
    enable_irq(PIT_IRQ); //enable the PIT
    outb(PIT_CMD, PIT_CMD_PORT); //set the command byte
    outb((PIT_FREQ/TAR_FREQ) & LSB_MASK, PIT_PORT); //set the LSB
    outb((PIT_FREQ/TAR_FREQ) >> MSB_SHIFT, PIT_PORT); //set the MSB
}

void pit_handler(void){
    int curr_term;
    int next_term;

    cli();
    //send EOI
    send_eoi(PIT_IRQ);

    if(curr_terminal[0] == -1){ //make new terminal if all are closed
        clear();
        //todo
    } 

    pcb_t *cur_pcb = get_pcb_from_pid(prog_counter - 1);


    next_term = get_next_terminal(cur_pcb->terminal_num);
    while(curr_terminal[next_term] == -1){
        next_term = get_next_terminal(next_term);
    }

    pcb_t *next_pcb = get_pcb_from_pid(curr_terminal[next_term]);

    set_up_pid_map(next_pcb->pid);
    

    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (PCB_SIZE * next_pcb->pid) - 10;




    sti();
    //switch terminals
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :
        :"=r"(cur_pcb->saved_esp), "=r"(cur_pcb->saved_ebp)
    );

    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "LEAVE;"
        "RET;"
        :"r"(next_pcb->saved_esp), "r"(next_pcb->saved_ebp)
    );

    return;
}