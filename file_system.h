#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "types.h"
#include "lib.h"

#define FILENAME_LEN 32
#define reserved_LEN_1 24
#define reserved_LEN_2 52
#define size_of_dir 63
#define size_data_block 1023
//reference: Lecture16
typedef struct{
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[reserved_LEN_1];
}dentry_t;

typedef struct { 
 	int32_t dir_count; 
 	int32_t inode_count;
    int32_t data_count;
    int8_t reserved[reserved_LEN_2];
    dentry_t direntries[size_of_dir]; 
 	int32_t flags; 
}boot_block_t;
extern boot_block_t* boot_block;
typedef struct{
    int32_t length;
    int32_t data_block_number[size_data_block];
}inode_t;

void fs_init(uint32_t addr);

int32_t file_open(const uint8_t* filename);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_close(int32_t fd);

int32_t dir_open(const uint8_t* filename);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_close(int32_t fd);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
