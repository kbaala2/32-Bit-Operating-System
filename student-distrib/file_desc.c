#include "file_desc.h"
#include "lib.h"
#include "rtc.h"

void init_fdarr(int32_t fd, void* buf, int32_t nbytes){
    //int32_t (*stdin_ptr)(int32_t, void*, int32_t) = &stdin;
    //int32_t (*stdout_ptr)(int32_t, const void*, int32_t) = &stdout;
    file_descriptor_array[0].jmp_pointer->read = stdin;
    file_descriptor_array[1].jmp_pointer->write = stdout;
    file_descriptor_array[0].jmp_pointer->read(fd,&buf,nbytes);
    file_descriptor_array[1].jmp_pointer->write(fd,&buf,nbytes);
}


void stdin(int32_t fd, void* buf, int32_t nbytes){
    if(fd != 0){ //check to make sure fd = 0 to indicate stdin
        return -1;
    }
    nbytes = 0;
    while(!nbytes){
        nbytes = terminal_read(0, buf, 128);
    }
    return;
}

int32_t stdout(int32_t fd, const void* buf, int32_t nbytes){
    if(fd != 1){ //check to make sure fd = 1 to indicate stdout
        return -1;
    }
    terminal_write(1, buf, nbytes);
    return nbytes;
}

int32_t open_handler(const uint8_t* filename){
    int i = 0;
    dentry_t dentry;

    if(filename == NULL){ //check if filename is null
        return -1;
    }
    
    if(read_dentry_by_name((filename, dentry)) == -1){  //check if filename is valid and fill dentry object
        return -1;
    }

    for(i = 2; i < 8; i++){
        if(file_descriptor_array[i].flag == 0){
            file_descriptor_array[i].flag = 1; //mark it as in use
            file_descriptor_array[i].inode_num = dentry.inode_num;
            file_descriptor_array[i].file_position = 0;
            switch (dentry.filetype)
            {
            case 0:
                /* code to say file is rtc, use jmp pointer */
                // file_descriptor_array.jmp_pointer = &rtc
                file_descriptor_array[i].jmp_pointer->open = rtc_open;
                file_descriptor_array[i].jmp_pointer->open(filename);
                return i;
            case 1:
                /* code to say file is dir, use jmp pointer */
                file_descriptor_array[i].jmp_pointer->open = open_d;
                file_descriptor_array[i].jmp_pointer->open(filename);
                return i;
            case 2:
                /* code to say file is file, use jmp pointer */
                file_descriptor_array[i].jmp_pointer->open = open_f;
                file_descriptor_array[i].jmp_pointer->open(filename);
                return i;
            default:
                return -1;
            }
            

        }
    }
    return -1; //no file descriptor free
}

int32_t read_handler(int32_t fd, void *buf, int32_t nbytes){

}

int32_t write_handler(int32_t fd, const void *buf, int32_t nbytes){
    if(fd < 1 || fd > 7) return -1; //invalid fd index
    
    if(fd == 1){ //write to terminal
    //TODO: find way to use function pointer instead of this boof method
        //return stdout(fd, buf, nbytes);
        file_descriptor_array[fd].jump_ptr->write = terminal_write;
        file_descriptor_array[fd].jump_ptr->write(fd,&buf,nbytes);

    } 
    if(file_descriptor_array[fd].inode_num != 0){ //we dont write to dir or files
        return -1;
    }

    if((file_descriptor_array[fd]) == 0){ //rtc write - this makes no sense
        //return rtc_write(fd, buf, nbytes); //TODO: change rtc_write return value
        file_descriptor_array[fd].jump_ptr->write = rtc_write;
        file_descriptor_array[fd].jump_ptr->write(fd,&buf,nbytes);
    }

    return -1;

}

int32_t close_handler(int32_t fd){
    if(fd == 0 || fd == 1){ //cant close stdout or stdin
        return -1;
    }
    if(fd < 1 || fd > 7){ //check if fd is within bounds
        return -1;
    }
    if(file_descriptor_array[fd].flag == 1){
        file_descriptor_array[fd].flag = 0;
        return 0;
    }
    return -1;

}