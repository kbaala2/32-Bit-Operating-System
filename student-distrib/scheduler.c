



#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"
#include "paging.h"
#include "terminal.h"
#include "sysCalls.h"
#include "scheduler.h"

#define _8MB 0x800000
#define PCB_SIZE 0x1000
#define _1MB 0x100000

int8_t prog_timer = 0;
int terminal_arr[MAX_TERMINALS];

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

    if(terminal_arr[0] == -1){ //make new terminal if all are closed
        clear();
        clear_pos();
        execute_terminal("shell", 0);
    } 

    pcb_t *cur_pcb = get_pcb_from_pid(pid);


    next_term = get_next_terminal(cur_pcb->terminal_idx);
    while(terminal_arr[next_term] == -1){
        next_term = get_next_terminal(next_term);
    }

    pcb_t* next_pcb = get_pcb_from_pid(terminal_arr[next_term]);

    set_up_pid_map(next_pcb->pid);
    set_up_vidmap_terminals(132 * _1MB, next_pcb->terminal_idx);
    

    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (0x2000 * next_pcb->pid) - sizeof(int32_t);
    pid = next_pcb->pid;
     set_act_terminal(next_pcb->terminal_idx);


    sti();

    //save current terminal esp and ebp
    asm volatile ("          \n\
                 movl %%ebp, %%eax  \n\
                 movl %%esp, %%ebx  \n\
            "
            :"=a"(cur_pcb->saved_ebp), "=b"(cur_pcb->saved_esp)
            );

    //load in next process esp and ebp
    asm volatile ("          \n\
                 movl %%ebx, %%esp  \n\
                 movl %%eax, %%ebp  \n\
                 leave           \n\
                 ret             \n\
            "
            ::"b"(next_pcb->saved_esp), "a"(next_pcb->saved_ebp)
            );

    return;
}
