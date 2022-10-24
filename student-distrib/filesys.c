#include "filesys.h"
#include "lib.h"

boot_block_t *origin;
data_block_t *first_data_block; 
inode_t *root_inode;
dentry_t* dentry_obj;
uint32_t inode_num;


int32_t file_init(boot_block_t *boot){
    origin = boot;
    root_inode = (origin + 1);
    inode_num = origin->inode_count;
    dentry_obj = &(origin->direntries[2]);
    first_data_block = (data_block_t*)(origin + inode_num + 1);//* 4096; // inode_num * sizeof(inode_t)
    return 0;
}


int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    int i = 0;
    if(index > ENTRY_NUM || index < 0) return -1;
    
    memcpy(dentry->filename, origin->direntries[index].filename, sizeof(dentry->filename));
    // dentry.filename = origin->direntries[idx].filename;
    dentry->filetype = origin->direntries[index].filetype;
    dentry->inode_num = origin->direntries[index].inode_num;
    // printf("%u",dentry->inode_num);
    // printf("%u",dentry->filetype);
    // for(i =0; i < 63; i++){
    //     if(dentry->filename[i] == NULL){
    //         break;
    //     }
    //     printf("%c", dentry->filename[i]);
    // }
    return 0;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int idx = 0; //file index
    int i;

    if(strlen((int8_t*)fname) > FILENAME_LEN || fname == NULL) return -1;

    for(idx = 0; idx < ENTRY_NUM; idx++){
        if(strncmp((int8_t*)origin->direntries[idx].filename,(int8_t*)fname, sizeof(fname)) == 0){
            // for(i =0; i < 63; i++){
            //     if(origin->direntries[idx].filename[i] == NULL){
            //         break;
            //     }
            //     printf("%c", origin->direntries[idx].filename[i]);
            // }
            // for(i =0; i < 63; i++){
            //     if(fname[i] == NULL){
            //         break;
            //     }
            //     printf("%c", fname[i]);
            // }
            //read_dentry_by_index(idx, dentry);
            //printf("%u",origin->direntries[idx].filetype);
            memcpy(dentry->filename, origin->direntries[idx].filename, sizeof(dentry->filename));
            // dentry.filename = origin->direntries[idx].filename;
            dentry->filetype = origin->direntries[idx].filetype;
            dentry->inode_num = origin->direntries[idx].inode_num;
            //printf("%u",dentry->filetype);
            // for(i =0; i < 63; i++){
            //     if(dentry->filename[i] == NULL){
            //         break;
            //     }
            //     printf("%c", dentry->filename[i]);
            // }
            return 0;
        }
    }
    
    return -1;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    inode_t* inode_ptr;
    uint32_t datab_num;
    data_block_t* data_block;
   // data_block_t* data_ptr;
    uint32_t data_block_offset;
    int i = 0;
    int end;
    uint32_t data_block_idx = offset/4096;
    if(inode >= inode_num || data_block_idx >= DATA_NUMBER) return -1;

    inode_ptr = root_inode + inode; // get inode from inode offset
    datab_num = inode_ptr->data_block_num[data_block_idx];  //get data block num from inode specified by offset
    // printf("%u", datab_num);
    data_block_offset = offset % 4096; //data block offset is the offset of the data block

    data_block = first_data_block + datab_num; // access the data block we want

    //data_ptr  = data_block->data_block_array;

    end = length; //end of length
    //return 0;

    for(i = 0; i < end; i++){
        if(i + data_block_offset == 4096){ //4096 is the last index of the current data block
            data_block_idx++; //go to next datablock
            data_block_offset = 0;
        }
        //memcpy(buf + i, data_block->data[i + data_block_offset], sizeof(data_block->data[i + data_block_offset]));
        buf[i] = data_block->data[i + data_block_offset];
    }
	// clear();

    // for(i = 0; i < 187; i++){
	// 	putc(buf[i]);
	// }

    return 0;
}

int32_t open_f(const uint8_t* filename){
    if(filename == NULL) return -1;
    return read_dentry_by_name(filename, dentry_obj);
}

int write_f(int32_t fd, const void *buf, int32_t nbytes){
    return -1;
}

int close_f(int32_t fd){
    // dentry_obj->filename = NULL;
    // dentry_obj->filetype = NULL;
    // dentry_obj->inode_num = NULL;
    return 0;
}

int read_f(int32_t fd, void *buf, int32_t nbytes){
    //TODO: read data
    return read_data(fd,0, buf, nbytes);
}

int32_t open_d(const uint8_t* filename){
    if(filename == NULL) return 0;
    read_dentry_by_name(filename, dentry_obj);
    if(dentry_obj->filetype ==1){
        return 0;
    }
    return -1;
}

int write_d(int32_t fd, const void *buf, int32_t nbytes){
    return -1;
}

int close_d(int32_t fd){
    return 0;
}

int read_d(int32_t fd, void *buf, int32_t nbytes){
    if(buf == NULL) return -1;
    if(fd > 62 || fd < 0 ) return -1;
    int i =0;
    //printf("%u",dentry_obj->filetype);
    for(i =0; i < 32; i++){
        if(origin->direntries[fd].filename[i] == NULL){
            continue;
        }
        //printf("%c", origin->direntries[fd].filename[i]);
    }
    //printf("\n");
    //read_dentry_by_index(fd, &(origin->direntries[fd]));
    memcpy(buf, origin->direntries[fd].filename, sizeof(origin->direntries[fd].filename));
    return 0;
}
