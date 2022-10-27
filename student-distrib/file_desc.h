#ifndef FILE_DESC_H
#define FILE_DESC_H

#include "types.h"
#include "filesys.h"
#include "rtc.h"
#include "filesys.h"
#include "terminal.h"

/*jmp_pointer->open = &open_f */
#define FD_SIZE 8

typedef struct file_operations_table {
    int (*open)(char* filename);
    int (*read)(int32_t fd, void *buf, int32_t nbytes);
    int (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int (*close)(int32_t fd);
}file_operations_table_t;

typedef struct __attribute__ ((packed)) file_descriptor {
    file_operations_table_t* jmp_pointer; 
    uint32_t inode_num;
    uint32_t file_position;
    uint8_t flag;
} file_descriptor_t;

file_descriptor_t file_descriptor_array[FD_SIZE];

extern void init_fdarr(file_descriptor_t* file_descriptor_array);
extern void stdin(int32_t fd, void* buf, int32_t nbytes);
extern int32_t stdout(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t open_handler(const uint8_t* filename);
extern int32_t write_handler(int32_t fd, const void *buf, int32_t nbytes);
extern int32_t close_handler(int32_t fd);
extern int32_t read_handler(int32_t fd, void *buf, int32_t nbytes);

#endif /* FILE_DESC_H */