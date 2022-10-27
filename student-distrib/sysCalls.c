#include "sysCalls.h"
#include "paging.h"
#include "filesys.h"

#define START_OF_USER 0x800000
#define START_OF_KERNEL 0x400000
#define KERNEL_STACK_SIZE 0x2000

int32_t prog_eip;
char buf[4];

void map_memory(int pid){
    pde[32].page_table_base_add = (((uint32_t) START_OF_USER) + (pid * 0x400000)) >> 12;
}

int check_executable(char* filename){
    dentry_t dentry;
    uint32_t file_len;
    int i;
    if(open_f(filename) == -1){
		return 0;
	}
    read_dentry_by_name(filename, &dentry);
    file_len = get_file_len(&dentry);
    read_data(dentry.inode_num, 0, ((uint32_t*)(0x08048000)), file_len);
    read_data(dentry.inode_num, 0, buf, 4);
    if((int)buf[0] != 0x7F){
        return 0;
    }
    if((int)buf[1] != 0x45){
        return 0;
    }
    if((int)buf[2] != 0x4C){
        return 0;
    }
    if((int)buf[3] != 0x46){
        return 0;
    }
    prog_eip = *0x80480C0;
    return 1;
}

int32_t sys_call_execute(const char* cmd){
    if(!check_executable(cmd)) {
        return -1;
    }
    struct pcb_t* pc = find_avail_pcb();
        pc.active = 1;
        pc.parent_pid = prog_counter;
        prog_counter++;
        pc.pid = prog_counter;

    map_memory(prog_counter);
}

struct pcb_t* find_avail_pcb(){
    int i = 0;
    for(i = START_OF_USER - KERNEL_STACK_SIZE; i > START_OF_KERNEL; i -= KERNEL_STACK_SIZE){
        struct pcb_t* prog = i;
        if(prog->active){
            return prog;
        }
    }
    return NULL;
}









