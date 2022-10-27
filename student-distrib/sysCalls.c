#include "sysCalls.h"
#include "paging.h"
#include "filesys.h"

#define START_OF_USER 0x800000

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

void 
