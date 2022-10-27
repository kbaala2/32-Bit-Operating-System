#include "file_desc.h"
#include "lib.h"


void init_fdarr(int32_t fd, void* buf, int32_t nbytes){
    int32_t (*stdin_ptr)(int32_t, void*, int32_t) = &stdin;
    int32_t (*stdout_ptr)(int32_t, const void*, int32_t) = &stdout;
    file_descriptor_array[0].jmp_pointer = (*stdin_ptr)(fd, buf, nbytes);
    file_descriptor_array[1].jmp_pointer = (*stdout_ptr)(fd, buf, nbytes);
}


void stdin(int32_t fd, void* buf, int32_t nbytes){
    nbytes = 0;
    while(!nbytes){
        nbytes = terminal_read(0, buf, 128);
    }
    return;
}

void stdout(int32_t fd, const void* buf, int32_t nbytes){
    terminal_write(0, buf, nbytes);
}