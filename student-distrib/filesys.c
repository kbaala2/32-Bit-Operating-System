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
    origin = boot;                                              // ptr to boot block
    root_inode = (origin + 1);                                  // ptr to first inode
    inode_num = origin->inode_count;                            // holds total number of inodes 
    dentry_obj = &(origin->direntries[2]);                      // ptr to 2nd dentry    
    first_data_block = (data_block_t*)(origin + inode_num + 1); //contains ptr to the first data block
    return 0;
}

/* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
 * Inputs: const uint8_t* fname, dentry_t* dentry
 * Return Value: -1 if not found, 0 if found
 * Function: reads dentry by index */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    int i = 0;
    if(index > ENTRY_NUM || index < 0) return -1;                                           //check if idx is within bounds
    
    /*copy to dentry object*/
    memcpy(dentry->filename, origin->direntries[index].filename, sizeof(dentry->filename)); //copies filename to dentry object
    dentry->filetype = origin->direntries[index].filetype;                                  //copies filetype to dentry object
    dentry->inode_num = origin->direntries[index].inode_num;                                //copies inode number to dentry object
    return 0;
}

/* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
 * Inputs: const uint8_t* fname, dentry_t* dentry
 * Return Value: -1 if not found, 0 if found
 * Function: reads dentry by name */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int idx = 0; //file index
    int i;

    if(strlen((int8_t*)fname) > FILENAME_LEN || fname == NULL) return -1; //check if the filename is not NULL and is at max 32 characters

    /* go through each dentry to check if the filename exists in the dentry, then copy to dentry object and return*/
    for(idx = 0; idx < ENTRY_NUM; idx++){
        if(strncmp((int8_t*)origin->direntries[idx].filename,(int8_t*)fname, sizeof(fname)) == 0){ 
            
            /*copy to dentry object*/
            memcpy(dentry->filename, origin->direntries[idx].filename, sizeof(dentry->filename));
            dentry->filetype = origin->direntries[idx].filetype;
            dentry->inode_num = origin->direntries[idx].inode_num;
            return 0;
        }
    }
    
    return -1; //return -1 when file name does not exist
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
    if(inode >= inode_num || data_block_idx >= DATA_NUMBER) return -1; //bounds checking

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
    return read_dentry_by_name(filename, dentry_obj); //open file by reading the filename
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
    return read_data(fd,0, buf, nbytes); //read file data in read file function
}

/* int32_t open_d(const uint8_t* filename);
 * Inputs: const uint8_t* filename
 * Return Value: -1 if not found, 0 if found
 * Function: opens directory */
int32_t open_d(const uint8_t* filename){
    if(filename == NULL) return 0;
    read_dentry_by_name(filename, dentry_obj);
    /*check if the stored dentry information is a directory type*/
    if(dentry_obj->filetype ==1){
        return 0;
    }
    return -1; //return -1 if file is not a directory
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
    if(buf == NULL) return -1;         // null check
    if(fd > 62 || fd < 0 ) return -1;  //check  if file index out of bound
    int i =0;

    /*loop to check how many bytes is contained in the filename*/
    for(i =0; i < 32; i++){
        if(origin->direntries[fd].filename[i] == NULL){
            nbytes = i;
            break;
        }
    }
    memcpy(buf, origin->direntries[fd].filename, sizeof(origin->direntries[fd].filename)); //copy filename to buffer
    return nbytes;
}

