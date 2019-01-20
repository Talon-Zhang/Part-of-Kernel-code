#include "file_system.h"
#include "lib.h"
#include "types.h"
#include "syscall.h"
static int directory_location = 0;
static int inode_addr;
boot_block_t* boot_block;

#define MAX_FILE 63
#define MAX_FILE_NAME 32
#define BLOCK_SIZE 4096 



/* void fs_init(uint32_t addr);
 * Inputs: uint32_t addr
 * Return Value:void
 * Function: init file system
 */
void fs_init(uint32_t addr)
{
    boot_block = (boot_block_t*)addr;
    inode_addr = (int)boot_block + BLOCK_SIZE;

}

/* int32_t file_open(const uint8_t* filename);
 * Inputs: const uint8_t* filename
 * Return Value:0
 * Function: open file
 */
int32_t file_open(const uint8_t* filename)
{
    return 0;
}

/* int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value:sum or -1;
 * Function: read file
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
{   
    int sum;
    pcb_t* pcb = get_pcb();

    if(buf==NULL){
        return FAIL;
    }



    sum = read_data(pcb->file_array[fd].inode,pcb->file_array[fd].file_pos,buf,nbytes);
    pcb->file_array[fd].file_pos+=sum;
    return sum;

}

/* int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value:sum or -1;
 * Function: 
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

/* int32_t file_close(int32_t fd)
 * Inputs: int32_t fd
 * Return: 0
 * Function: rerset the file position in current pdb
 */
int32_t file_close(int32_t fd)
{
    
    pcb_t* pcb = get_pcb();
    
    
    pcb->file_array[fd].file_pos = 0;
    return 0;
}

/* int32_t dir_open(const uint8_t* filename)
 * Inputs: const uint8_t* filename
 * Return Value:0;
 * Function: open file
 */
int32_t dir_open(const uint8_t* filename)
{
    return 0;
}

/* int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: the length of the file read;
 * Function: read and copy the content in the file according to the offset and number of bytes to the buffer
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
    if(directory_location >= boot_block->dir_count)
    {
        directory_location = 0;
        return 0;
    }
    else
    {
        if(nbytes > MAX_FILE_NAME)
            nbytes = MAX_FILE_NAME;

        strncpy((int8_t *)buf,(const int8_t *)boot_block->direntries[directory_location].filename,nbytes);
        directory_location += 1;
        return strlen((int8_t *)buf);
    }

    return 0;
}

/* int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value:-1;
 * Function: should not be used
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

/* int32_t dir_close(int32_t fd)
 * Inputs: int32_t fd
 * Return Value:0;
 * Function: do not need to do anything
 */
int32_t dir_close(int32_t fd)
{
    return 0;
}

/* int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
 * Inputs: const uint8_t* fname, dentry_t* dentry
 * Return Value: 0 for success and -1 for failure
 * Function: get the information of the dentry according to the input fname which is the file name we want to find
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
    int i;
    if (fname == NULL || dentry == NULL)
    {
        return -1;
    }
    if(strlen((const char*)fname) == 0){
        return -1;
    }

    for(i = 0;i < MAX_FILE; i++)
    {
        if (strncmp((int8_t*)fname,boot_block->direntries[i].filename,MAX_FILE_NAME) == 0)
        {
            if(strlen(boot_block->direntries[i].filename) >= MAX_FILE_NAME)
            {
                strncpy(dentry->filename, boot_block->direntries[i].filename, MAX_FILE_NAME);
            }
            else
            {
                strcpy(dentry->filename,boot_block->direntries[i].filename);
            }
            dentry->inode_num = boot_block->direntries[i].inode_num;
            dentry->filetype =  boot_block->direntries[i].filetype;
            return 0;
        }
    }

    return -1;
}

/* int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
 * Inputs: uint32_t index, dentry_t* dentry
 * Return Value: 0 for success and -1 for failure
 * Function: get the information of the dentry according to the input index which is the file index we want to find
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
    if (dentry == NULL)
    {
        return -1;
    }
    if (index < 0 || index >= boot_block->dir_count)
    {
       return -1;
    }
    else
    {
        if(strlen(boot_block->direntries[index].filename) >= MAX_FILE_NAME)
        {
            strncpy(dentry->filename, boot_block->direntries[index].filename, MAX_FILE_NAME);
        }
        else
        {
            strcpy(dentry->filename,boot_block->direntries[index].filename);
        }
        dentry->filetype = boot_block->direntries[index].filetype;
        dentry->inode_num = boot_block->direntries[index].inode_num;

        return 0;
    }

    return -1;
}

/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
 * Inputs: uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length
 * Return Value:number of byte we read or -1 if fail to read the data correctly
 * Function: read the data of the file with offset, the number of bytes we want to read is decided by the input length
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    //error checking
    if (inode >= boot_block->inode_count || inode < 0 || buf == NULL)
        return -1;

    //initialization
    uint32_t i,j;
    uint32_t bytes_copy;
    inode_t* cur_inode = (inode_t*)(inode_addr + inode * BLOCK_SIZE);

    //condition check again
    if (length == 0)
        return 0;

    if (offset >= cur_inode->length)
    {
        buf[0] = '\0';
        return 0;
    }

    //determine the number of bytes we nned to read
    if (offset + length <= cur_inode->length)
    {
        bytes_copy = length;
    }
    else
    {
        bytes_copy = cur_inode->length - offset;
    }

    //initialization for copy the data of the file
    uint32_t num_block = (offset + length) / BLOCK_SIZE;
    uint32_t bytes_remiander = (offset + length) % BLOCK_SIZE;
    uint32_t data_index = offset % BLOCK_SIZE;
    uint32_t num_block_offset = offset / BLOCK_SIZE;
    uint32_t l;
    num_block += 1;
    num_block_offset += 1;

    //check whether the data block number is valid or not
    for (i = num_block_offset; i <= num_block; i++)
    {
        if (cur_inode->data_block_number[i - 1] > boot_block->data_count || (cur_inode->data_block_number[i - 1] < 0))
            return -1;
    }

    uint8_t data;
    if (num_block > num_block_offset)
    {
        i = 0;
        while(i < BLOCK_SIZE - data_index){
            l = (boot_block->inode_count + cur_inode->data_block_number[num_block_offset - 1]) * BLOCK_SIZE;
            data = *(char*)(inode_addr + l + data_index + i);
            *(char*)((int)buf + i) = data;
            i++;
        }

        i = num_block_offset + 1;
        while(i <= num_block - 1){
            j = 0;
            while(j < BLOCK_SIZE){
                l = (boot_block->inode_count + cur_inode->data_block_number[i - 1])*BLOCK_SIZE;
                data = *(char*)(inode_addr + l + j);
                *(char*)((int)buf + (i - num_block_offset) * BLOCK_SIZE - data_index + j) = data;
                j++;
            }
            i++;
        }

        j = 0;
        while(j < bytes_remiander){
            l = (boot_block->inode_count + cur_inode->data_block_number[i - 1])*BLOCK_SIZE;
            data = *(char*)(inode_addr + l + j);
            *(char*)((int)buf + (i - num_block_offset) * BLOCK_SIZE - data_index + j) = data;
            j++;
        }
    }
    else
    {
        i = 0;
        while(i < (bytes_remiander - data_index)){
            l = (boot_block->inode_count + cur_inode->data_block_number[num_block_offset - 1])*BLOCK_SIZE;
            data = *(char*)(inode_addr + l + data_index + i);
            *(char*)((int)buf + i) = data;
            i++;
        }
    }
    return bytes_copy;
}

