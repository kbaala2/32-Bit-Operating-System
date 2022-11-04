#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "types.h"
#include "filesys.h"
#include "rtc.h"
#include "filesys.h"
#include "terminal.h"

int check_executable(char* filename);
void map_memory(int pid);
void init_file_operations();

extern int32_t sys_execute(const char* cmd);
extern void stdin(int32_t fd, void* buf, int32_t nbytes);
extern int32_t stdout(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t sys_open(const uint8_t* filename);
extern int32_t sys_write(int32_t fd, const void *buf, int32_t nbytes);
extern int32_t sys_close(int32_t fd);
extern int32_t sys_read(int32_t fd, void *buf, int32_t nbytes);

typedef struct file_operations_table {
    int32_t (*open)(const uint8_t* filename);
    int32_t (*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int32_t (*close)(int32_t fd);
}file_operations_table_t;

file_operations_table_t stdin_file_op;
file_operations_table_t stdout_file_op;
file_operations_table_t rtc_file_op;
file_operations_table_t directory_file_op;
file_operations_table_t file_file_op;
file_operations_table_t bad_file_op;

typedef struct __attribute__ ((packed)) file_descriptor {
    file_operations_table_t* jmp_pointer;
    uint32_t inode_num;
    uint32_t file_position;
    uint8_t flag;
} file_descriptor_t;

typedef struct __attribute__ ((packed)) pcb {
    int pid;
    int parent_pid;
    uint32_t saved_esp;
    uint32_t saved_ebp;
    int active;
    file_descriptor_t fd_arr[8];
} pcb_t;

pcb_t* create_pcb(int pid);
pcb_t* get_pcb();

#endif /* _SYSCALLS_H */
