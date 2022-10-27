#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "file_desc.h"

int check_executable(char* filename);
void map_memory(int pid);
void sys_call_execute(const char* cmd);
extern int prog_counter = 0;
pcb_t* find_avail_pcb();

typedef struct __attribute__ ((packed)) pcb_t{
    int pid;
    int parent_id;
    file_descriptor_t file_descriptor_array[8];
    uint32_t saved_esp;
    uint32_t saved_ebp;
    int active;
} pcb_t;

#endif /* _SYSCALLS_H */