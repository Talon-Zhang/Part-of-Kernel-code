#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"

#define MAXFILE 8
#define USABLE_FD 2
#define FAIL    -1
#define DAFAULT_FD -1
#define RTC 0
#define DIR 1
#define FILE 2
#define MAXFILE 8
#define MASK_13 13
#define NUM_32 32
#define NUM_128 128
#define MAGIC_1 0x7f
#define MAGIC_2 0x45
#define MAGIC_3 0x4c
#define MAGIC_4 0x46
#define INDEX_0 0
#define INDEX_1 1
#define INDEX_2 2
#define INDEX_3 3
#define SIZE_4 4
#define SIZE_6 6
#define MB8 0x800000
#define MB4 0x400000
#define KB8 0x40000
#define B_SIZE 4096
#define MAGIC 294912
#define STARTADDR 0x8048000
#define ADDR_132MB 0x08400000
#define ADDR_128MB 0x08000000
#define PHYS_MEM 0xB8000


/* structure of file operations */
// typedef struct{
//     int32_t (*read)(int32_t fd,void * buf,int32_t nbytes);
//     int32_t (*write)(int32_t fd,const void * buf,int32_t nbytes);
//     int32_t (*open) (const uint8_t* filename);
//     int32_t (*close) (int32_t fd);
// }fops_t;
typedef int32_t (*fops_t)();

/* structure of file_desciptor */
typedef struct {
    /* file operation pointer */
    fops_t f_ops[4];
    uint32_t inode;
    uint32_t file_pos;
    /* flag for open file */
    uint32_t flag;
}file_descriptor_t;
/* structure of pcb */

typedef struct {
    /* array to store file */
    file_descriptor_t file_array[MAXFILE];
    /* current pid */
    uint32_t pid;
    /* parent element */
    uint32_t esp0;
    uint32_t parent_pid;
    uint32_t parent_esp;
    uint32_t parent_ebp;
    /* return address */
    uint32_t ret_addr;
    uint32_t status_halt;

}pcb_t;
extern int32_t halt (uint8_t status);

extern int32_t execute (const uint8_t* command);

extern int32_t read (int32_t fd,void * buf,int32_t nbytes);

extern int32_t write (int32_t fd,const void * buf,int32_t nbytes);

extern int32_t open (const uint8_t* filename);

extern int32_t close (int32_t fd);

extern int32_t getargs (void * buf,int32_t nbytes);

extern int32_t vidmap (uint8_t** screen_start);

extern int32_t set_handler(int32_t signum,void * handler_address);

extern int32_t sigreturn(void);

extern pcb_t* get_pcb();


extern void init_syscall();


//helper functions for execute
int32_t parse(uint8_t* file_name, uint8_t* argument, const uint8_t* command);
int32_t check_executable(uint8_t* file_name);
int32_t get_first_available_index();
int32_t program_loader(int location);
void get_addr();
void flush_tlb();
void create_pcb(pcb_t* cur);
void context_switch(pcb_t* pcb);


#endif
