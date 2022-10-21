#ifndef FILESYS_H
#define FILESYS_H

#define FILENAME_LEN 32
#define ENTRY_NUM 63
#define DATA_NUMBER 1023
#define RESERVED_LEN 24
#define RESERVED_BOOT 52

typedef struct boot_block{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    uint8_t reserved[RESERVED_BOOT];
    dentry_t direntries[ENTRY_NUM];
}boot_block_t;

typedef struct dentry{
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[RESERVED_LEN];
}dentry_t;

typedef struct inode{
    uint32_t length;
    uint32_t data_block_num[DATA_NUMBER];
}inode_t;

typedef struct data_block{
    uint32_t data_block_array*;
}data_block_t;

extern int32_t file_init(boot_block_t *boot);
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern int32_t open_f(const uint8_t* filename);
extern int write_f(int32_t fd, const void *buf, int32_t nbytes);
extern int close_f(int32_t fd);
extern int read_f(int32_t fd, void *buf, int32_t nbytes);

extern int32_t open_d(const uint8_t* filename);
extern int write_d(int32_t fd, const void *buf, int32_t nbytes);
extern int close_d(int32_t fd);
extern int read_d(int32_t fd, void *buf, int32_t nbytes);


extern boot_block_t *boot; 

#endif /* FILESYS_H */