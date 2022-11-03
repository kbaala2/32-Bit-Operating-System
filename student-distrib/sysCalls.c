#include "sysCalls.h"
#include "x86_desc.h"
#include "filesys.h"

#define START_OF_USER 0x800000
#define START_OF_KERNEL 0x400000
#define KERNEL_STACK_SIZE 0x2000

uint32_t prog_eip;
char buf[4];
uint32_t eflag_bitmask = 0x200;
uint32_t stack_pointer = 0x08400000;
char* buffer;
extern void flush_tlb();
extern void goto_user(uint32_t SS, uint32_t ESP, uint32_t eflag_bitmask, uint32_t CS, uint32_t EIP);

void map_memory(int pid){
    pde[32].page_table_base_add = (((uint32_t) START_OF_USER) + (pid * 0x400000)) >> 12;
    flush_tlb(); //flushing the TLB
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
    int i = 0;
    while(cmd[i] != '\n' || cmd[i] != '\0' || cmd[i] != ' '){
        buffer[i] = cmd[i];
        i++;
    }
    if(!check_executable(buffer)) {
        return -1;
    }
    map_memory(prog_counter);
    struct pcb_t pc = create_pcb(prog_counter);
        pc.active = 1;
        if(prog_counter == 0){
            pc.parent_pid = NULL;
        }
        else{
            pc.parent_pid = prog_counter - 1;
        }
        pc.pid = prog_counter;
        
        prog_counter++;
        register uint32_t pc.saved_ebp asm("ebp");  //take a look later
        register uint32_t pc.saved_esp asm("esp");
        pc.file_descriptor_array[0].flag = 1;
        pc.file_descriptor_array[1].flag = 1;
        pc.file_descriptor_array[0].jmp_pointer = ; //fill with terminal read and write
        pc.file_descriptor_array[1].jmp_pointer = ;
    tss.esp0 = START_OF_USER - (prog_counter * KERNEL_STACK_SIZE);
    //create and call asm linkage function for iret (userspace)
    goto_user(USER_DS, stack_pointer, eflag_bitmask, USER_CS, prog_eip);
}

struct pcb_t create_pcb(int pid){
    int i;
    struct pcb_t prog;
    i = START_OF_USER - ((pid+1) * KERNEL_STACK_SIZE);
    *i = prog;
    return prog;
}









