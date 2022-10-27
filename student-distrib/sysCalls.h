#ifndef _SYSCALLS_H
#define _SYSCALLS_H

int check_executable(char* filename);
void map_memory(int pid);

typedef struct __attribute__ ((packed)) pcb_t{
    int pid;
    int parent_id;
    file_descriptor_t file_desc_arr[8];
    uint32_t saved_esp;
    uint32_t saved_ebp;
    int active;
} pcb_t;

#endif /* _SYSCALLS_H */