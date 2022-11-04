#include "sysCalls.h"
#include "x86_desc.h"
#include "filesys.h"
#include "lib.h"

#define START_OF_USER   0x800000
#define START_OF_KERNEL 0x400000
#define KERNEL_STACK_SIZE 0x2000

uint32_t prog_eip;
uint8_t buf[4];
uint32_t eflag_bitmask = 0x200;
uint32_t stack_pointer = 0x08400000 - sizeof(int32_t);
char buffer[64];
extern void flush_tlb();
extern void goto_user(uint32_t SS, uint32_t ESP, uint32_t eflag_bitmask, uint32_t CS, uint32_t EIP);
uint32_t dir_index = 0;
int prog_counter = 0;
int bad_call();

void map_memory(int pid){
    pde[32].page_table_base_addr = (((uint32_t) START_OF_USER) + (pid * 0x400000)) >> 12;
    pde[32].user_supervisor = 1;
    pde[32].present = 1;
    pde[32].page_size = 1;
    flush_tlb(); //flushing the TLB
}

int check_executable(char* filename){
    dentry_t dentry;
    uint32_t file_len;
    
    if(open_f((const uint8_t*)filename) == -1){
		return 0;
	}
    read_dentry_by_name((const uint8_t*)filename, &dentry);
    file_len = get_file_len(&dentry);
    uint8_t temp_buf[file_len];
    read_data(dentry.inode_num, 0, temp_buf, file_len);
    read_data(dentry.inode_num, 0, buf, 4);
    memcpy(((uint32_t*)(0x08048000)), temp_buf, sizeof((temp_buf)));
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
    int32_t* prog_image;
    prog_image = 0x8048018;
    prog_eip = (uint32_t)(*(prog_image));
    return 1;
}

int32_t sys_execute(const char* cmd){
    cli();
    int i = 0;
    int j;
    while(cmd[i] != '\n' && cmd[i] != '\0' && cmd[i] != ' '){
        buffer[i] = cmd[i];
        i++;
    }
    map_memory(prog_counter);
    if(!check_executable(buffer)) {
        return -1;
    } 
    pcb_t* pc;
    pc = create_pcb(prog_counter);
        pc->active = 1;
        if(prog_counter == 0){
            pc->parent_pid = NULL;
        }
        else{
            pc->parent_pid = prog_counter - 1;
        }
        pc->pid = prog_counter;
        
        prog_counter++;
        asm volatile ("          \n\
                     movl %%ebp, %%eax  \n\
                     movl %%esp, %%ebx  \n\
                "
                :"=a"(pc->saved_ebp), "=b"(pc->saved_esp)
                );
        // register uint32_t pc.saved_ebp asm("ebp");  //take a look later
        // register uint32_t pc.saved_esp asm("esp");
        pc->fd_arr[0].flag = 1;
        pc->fd_arr[1].flag = 1;
        pc->fd_arr[0].inode_num = 0;
        pc->fd_arr[1].inode_num = 0;
        pc->fd_arr[0].file_position = 0;
        pc->fd_arr[0].file_position = 0;
        
        init_file_operations();
        pc->fd_arr[0].jmp_pointer = &stdin_file_op;
        //pc->fd_arr[0].jmp_pointer->read(0,buf,0);
        pc->fd_arr[1].jmp_pointer = &stdout_file_op;
        //pc->fd_arr[1].jmp_pointer->write(1,&buf,128);
        for(j = 2; j < 8; j++){
            pc->fd_arr[i].flag = 0;
            pc->fd_arr[i].inode_num = 0;
            pc->fd_arr[i].file_position = 0;
            pc->fd_arr[i].jmp_pointer = &bad_file_op;
        }
    tss.esp0 = START_OF_USER - ((prog_counter-1) * KERNEL_STACK_SIZE) - sizeof(int32_t);
    //create and call asm linkage function for iret (userspace)
    goto_user(USER_DS, stack_pointer, eflag_bitmask, USER_CS, prog_eip);
    sti();
    return 0;
}

void init_file_operations(){
    stdin_file_op.open = &terminal_open;
    stdin_file_op.write = &bad_call;
    stdin_file_op.close = &terminal_close;
    stdin_file_op.read = &terminal_read;

    stdout_file_op.open = &terminal_open;
    stdout_file_op.write = &terminal_write;
    stdout_file_op.close = &terminal_close;
    stdout_file_op.read = &bad_call;

    rtc_file_op.open = &rtc_open;
    rtc_file_op.write = &rtc_write;
    rtc_file_op.close = &rtc_close;
    rtc_file_op.read = &rtc_read;

    directory_file_op.open = &open_d;
    directory_file_op.write = &write_d;
    directory_file_op.close = &close_d;
    directory_file_op.read = &read_d;

    file_file_op.open = &open_f;
    file_file_op.write = &write_f;
    file_file_op.close = &close_f;
    file_file_op.read = &read_f;

    bad_file_op.open = &bad_call;
    bad_file_op.write = &bad_call;
    bad_file_op.close = &bad_call;
    bad_file_op.read = &bad_call;
}

int bad_call(){
    return -1;
}

pcb_t* create_pcb(int pid){
    pcb_t* i;
    i = (pcb_t*)(START_OF_USER - ((pid+1) * KERNEL_STACK_SIZE));
    return i;
}

pcb_t* get_pcb(){
    pcb_t* cur_pcb;
    cur_pcb = (pcb_t*)(START_OF_USER - ((prog_counter+1) * KERNEL_STACK_SIZE));
    return cur_pcb;
}

// void stdin(int32_t fd, void* buf, int32_t nbytes){
//     if(fd != 0){ //check to make sure fd = 0 to indicate stdin
//         return;
//     }
//     nbytes = 0;
//     while(!nbytes){
//         nbytes = terminal_read(0, buf, 128);
//     }
//     return;
// }

// int32_t stdout(int32_t fd, const void* buf, int32_t nbytes){
//     if(fd != 1){ //check to make sure fd = 1 to indicate stdout
//         return -1;
//     }
//     terminal_write(1, buf, nbytes);
//     return nbytes;
// }

int32_t sys_halt(uint8_t status){
    return 0;
}

int32_t sys_open(const uint8_t* filename){
    init_file_operations();
    int i = 0;
    dentry_t dentry;

    if(filename == NULL){ //check if filename is null
        return -1;
    }
    
    if(read_dentry_by_name(filename, &dentry) == -1){  //check if filename is valid and fill dentry object
        return -1;
    }

    pcb_t* pc_cur = get_pcb();

    for(i = 2; i < 8; i++){
        if(pc_cur->fd_arr[i].flag == 0){
            pc_cur->fd_arr[i].flag = 1; //mark it as in use
            pc_cur->fd_arr[i].inode_num = dentry.inode_num;
            pc_cur->fd_arr[i].file_position = 0;
            switch (dentry.filetype)
            {
            case 0:
                /* code to say file is rtc, use jmp pointer */
                // pc_cur.fd_arr.jmp_pointer = &rtc
                pc_cur->fd_arr[i].jmp_pointer = &rtc_file_op;
                pc_cur->fd_arr[i].jmp_pointer->open(filename);
                return i;
            case 1:
                /* code to say file is dir, use jmp pointer */
                pc_cur->fd_arr[i].jmp_pointer = &directory_file_op;
                pc_cur->fd_arr[i].jmp_pointer->open(filename);
                return i;
            case 2:
                /* code to say file is file, use jmp pointer */
                pc_cur->fd_arr[i].jmp_pointer = &file_file_op;
                pc_cur->fd_arr[i].jmp_pointer->open(filename);
                return i;
            default:
                return -1;
            }
        }
    }
    return -1; //no file descriptor free
}

int32_t sys_read(int32_t fd, void *buf, int32_t nbytes){
    init_file_operations();
    int filetype;

    if(fd < 0 || fd > 7) return -1; //invalid fd index

    if(fd == 1) return -1;

    pcb_t* pc_cur = get_pcb();

    if(fd == 0) {
        pc_cur->fd_arr[fd].jmp_pointer = &stdin_file_op;
        pc_cur->fd_arr[fd].jmp_pointer->read(fd, buf, nbytes);
    }

    filetype = get_filetype_from_inode(pc_cur->fd_arr[fd].inode_num);
    if(filetype == -1) return -1; //invalid file

    if(filetype == 2) {
        pc_cur->fd_arr[fd].jmp_pointer = &file_file_op;
        pc_cur->fd_arr[fd].jmp_pointer->read(fd, buf, nbytes);
        pc_cur->fd_arr[fd].file_position += nbytes;
    }
    if(filetype == 1){
        pc_cur->fd_arr[fd].jmp_pointer = &directory_file_op;
        pc_cur->fd_arr[fd].jmp_pointer->read(fd, buf, nbytes);
        dir_index++; //find way to differentiate between direntry and RTC files
    }
    else{ //file is rtc
        pc_cur->fd_arr[fd].jmp_pointer = &rtc_file_op;
        pc_cur->fd_arr[fd].jmp_pointer->read(fd, buf, nbytes);
    }
    return 0;
}

int32_t sys_write(int32_t fd, const void *buf, int32_t nbytes){
    init_file_operations();
    int filetype;
    if(fd < 1 || fd > 7) return -1; //invalid fd index

    pcb_t* pc_cur = get_pcb();
    
    if(fd == 1){ //write to terminal
    //TODO: find way to use function pointer instead of this boof method
        //return stdout(fd, buf, nbytes);
        pc_cur->fd_arr[fd].jmp_pointer = &stdout_file_op;
        pc_cur->fd_arr[fd].jmp_pointer->write(fd, buf, nbytes);

    } 
    filetype = get_filetype_from_inode(pc_cur->fd_arr[fd].inode_num);
    if(filetype != 0){ //we dont write to dir or files
        return -1;
    }
    if(filetype == 0){ //rtc write - this makes no sense
        //return rtc_write(fd, buf, nbytes); //TODO: change rtc_write return value
        pc_cur->fd_arr[fd].jmp_pointer = &rtc_file_op;
        pc_cur->fd_arr[fd].jmp_pointer->write(fd, buf, nbytes);
    }
    return -1;
}

int32_t sys_close(int32_t fd){
    init_file_operations();
    if(fd == 0 || fd == 1){ //cant close stdout or stdin
        return -1;
    }
    if(fd < 1 || fd > 7){ //check if fd is within bounds
        return -1;
    }

    pcb_t* pc_cur = get_pcb();

    if(pc_cur->fd_arr[fd].flag == 1){
        pc_cur->fd_arr[fd].flag = 0;
        return 0;
    }
    return -1;
}
