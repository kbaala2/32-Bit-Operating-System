#include "filesys.h"
#include "lib.h"

boot_block_t *origin;
data_block_t *first_data_block; 
inode_t *root_inode;
dentry_t* dentry_obj;
uint32_t inode_num;


/* int32_t file_init(boot_block_t *boot);
 * Inputs: boot_block_t *boot
 * Return Value: 0
 * Function: inits filesystem */
int32_t file_init(boot_block_t *boot){
    origin = boot;
    root_inode = (origin + 1);
    inode_num = origin->inode_count;
    dentry_obj = &(origin->direntries[2]);
    first_data_block = (data_block_t*)(origin + inode_num + 1);//* 4096; // inode_num * sizeof(inode_t)
    return 0;
}

/* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
 * Inputs: const uint8_t* fname, dentry_t* dentry
 * Return Value: -1 if not found, 0 if found
 * Function: reads dentry by index */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    int i = 0;
    if(index > ENTRY_NUM || index < 0) return -1;
    
    memcpy(dentry->filename, origin->direntries[index].filename, sizeof(dentry->filename));
    dentry->filetype = origin->direntries[index].filetype;
    dentry->inode_num = origin->direntries[index].inode_num;
    return 0;
}

/* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
 * Inputs: const uint8_t* fname, dentry_t* dentry
 * Return Value: -1 if not found, 0 if found
 * Function: reads dentry by name */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int idx = 0; //file index
    int i;

    if(strlen((int8_t*)fname) > FILENAME_LEN || fname == NULL) return -1;

    for(idx = 0; idx < ENTRY_NUM; idx++){
        if(strncmp((int8_t*)origin->direntries[idx].filename,(int8_t*)fname, sizeof(fname)) == 0){
            
            memcpy(dentry->filename, origin->direntries[idx].filename, sizeof(dentry->filename));
            dentry->filetype = origin->direntries[idx].filetype;
            dentry->inode_num = origin->direntries[idx].inode_num;
            return 0;
        }
    }
    
    return -1;
}

/* int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 * Inputs: uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length
 * Return Value: -1 if not found, 0 if found
 * Function: reads data */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    inode_t* inode_ptr;
    uint32_t datab_num;
    data_block_t* data_block;
    uint32_t data_block_offset;
    int i = 0;
    int end;
    uint32_t data_block_idx = offset/4096;
    if(inode >= inode_num || data_block_idx >= DATA_NUMBER) return -1;

    inode_ptr = root_inode + inode; // get inode from inode offset
    datab_num = inode_ptr->data_block_num[data_block_idx];  //get data block num from inode specified by offset
    data_block_offset = offset % 4096; //data block offset is the offset of the data block

    data_block = first_data_block + datab_num; // access the data block we want
    end = length; //end of length

    for(i = 0; i < end; i++){
        if(i + data_block_offset == 4096){ //4096 is the last index of the current data block
            data_block_idx++; //go to next datablock
            data_block_offset = 0;
        }
        buf[i] = data_block->data[i + data_block_offset];
    }

    return 0;
}

/* int32_t open_f(const uint8_t* filename);
 * Inputs: const uint8_t* filename
 * Return Value: -1 if not found, 0 if found
 * Function: opens file */
*/
int32_t open_f(const uint8_t* filename){
    if(filename == NULL) return -1;
    return read_dentry_by_name(filename, dentry_obj);
}

/* int32_t write_f(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1 
 * Function: writes file */
int write_f(int32_t fd, const void *buf, int32_t nbytes){
    return -1;
}

/* int32_t close_f(int32_t fd);
 * Inputs: int32_t fd
 * Return Value: -1 
 * Function: closes file */
int close_f(int32_t fd){
    
    return 0;
}

/* int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1 if not found, 0 if found
 * Function: reads file */
int read_f(int32_t fd, void *buf, int32_t nbytes){
   
    return read_data(fd,0, buf, nbytes);
}

/* int32_t open_d(const uint8_t* filename);
 * Inputs: const uint8_t* filename
 * Return Value: -1 if not found, 0 if found
 * Function: opens directory */
int32_t open_d(const uint8_t* filename){
    if(filename == NULL) return 0;
    read_dentry_by_name(filename, dentry_obj);
    if(dentry_obj->filetype ==1){
        return 0;
    }
    return -1;
}

/* int32_t write_d(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1 
 * Function: writes directory */
int write_d(int32_t fd, const void *buf, int32_t nbytes){
    return -1;
}

/* int32_t close_d(int32_t fd);
 * Inputs: int32_t fd
 * Return Value: -1 
 * Function: closes directory */
int close_d(int32_t fd){
    return 0;
}

/* int32_t read_d(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1 if not found, 0 if found
 * Function: reads directory */
int read_d(int32_t fd, void *buf, int32_t nbytes){
    if(buf == NULL) return -1;
    if(fd > 62 || fd < 0 ) return -1;  //check  if file index out of bound
    int i =0;

    for(i =0; i < 32; i++){
        if(origin->direntries[fd].filename[i] == NULL){
            nbytes = i;
            break;
        }
    }
    memcpy(buf, origin->direntries[fd].filename, sizeof(origin->direntries[fd].filename));
    return nbytes;
}

