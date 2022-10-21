#include "filesys.h"
#include "lib.h"

boot_block_t *origin;
data_block_t *first_data_block; 
inode_t *root_inode;
dentry_t* dentry_obj;
uint32_t inode_num;
uint32_t data_num;

int32_t file_init(boot_block_t *boot){
    origin = boot;
    root_inode = (inode_t*)(origin + 1);
    inode_num = origin->inode_count;
    dentry_obj = (dentry_t*)(origin.direntries[2]);
    data_num = origin->data_num;
    first_data_block = inode_num * 4096; // inode_num * sizeof(inode_t)

}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int idx = 0; //file index

    if(strlen(fname) > FILENAME_LEN) return -1;

    for(idx = 0; i < ENTRY_NUM; i++){
        if(strncmp(origin->direntries[i].filename,fname)){
            return read_dentry_by_index(idx, dentry);
        }
    }

    return -1;
}
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    if(index > ENTRY_NUM || index < 0) return -1;
    memcpy(dentry->filename, origin->direntries[idx].filename, sizeof(dentry->filename));
    // dentry.filename = origin->direntries[idx].filename;
    dentry->filetype = origin->direntries[idx].filetype;
    dentry->inode_num = origin->direntries[idx].inode_num;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    inode_t* inode_ptr;
    data_block_t* datab_num;
    data_block_t* data_block;
    uint32_t data_block_offset;
    int i = 0;
    int temp_idx;
    uint8_t* copy_buf;
    uint32_t data_block_idx = offset/4096;
    
    if(inode >= inode_num || data_block_idx >= DATA_NUMBER) return -1;

    inode_ptr = root_inode + inode; // get inode from inode offset
    datab_num = inode_ptr->data_block_num[data_block_idx];  //get data block num from inode specified by offset
    data_block_offset = offset % 4096; //data block offset is the offset of the data block

    data_block = first_data_block + datab_num; // access the data block we want

    for(i = data_block_offset; i < length + data_block_offset; i++){
        if(i == 4096){ //4096 is the last index of the current data block
            
        }    
    }



}

int32_t open_f(const uint8_t* filename){
    return read_dentry_by_name(filename, dentry_obj);
}

int write_f(int32_t fd, const void *buf, int32_t nbytes){
    return -1;
}

int close_f(int32_t fd){
    dentry_obj->filename = NULL;
    dentry_obj->filetype = NULL;
    dentry_obj->inode_num = NULL;
    return 0;
}

int read_f(int32_t fd, void *buf, int32_t nbytes){
    //TODO: read data
}

int32_t open_d(const uint8_t* filename){
    read_dentry_by_name(filename, dentry_obj);
    if(dentry_obj->filetype ==1){
        return 0;
    }
    return -1;
}

int write_d(int32_t fd, const void *buf, int32_t nbytes){
    return -1;
}

int close_f(int32_t fd){
    return 0;
}

int read_d(int32_t fd, void *buf, int32_t nbytes){
    int read_idx = 0;
    read_dentry_by_index(read_idx, dentry_obj);
    memcpy(buf, dentry_obj->filename, sizeof(dentry_obj->filename));
    return 0;
}