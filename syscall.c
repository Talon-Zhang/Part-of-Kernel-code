#include "types.h"
#include "syscall.h"
#include "int_handler.h"
#include "rtc.h"
#include "file_system.h"
#include "x86_desc.h" 
#include "paging.h"
/* file operation for stdin, stdout, rtc, directory and file */
fops_t stdin_fops[SIZE_4];
fops_t stdout_fops[SIZE_4];
fops_t rtc_fops[SIZE_4]; 
fops_t dir_fops[SIZE_4]; 
fops_t file_fops[SIZE_4];
int pcb_esp;
int process[SIZE_6] = {0,0,0,0,0,0};
uint8_t arg_buf[NUM_128];
int load_addr;
dentry_t dentry;
int parent_pid = -1;




/* int32_t init_syscall(void);
 * Inputs: void
 * Return Value:void
 * Function: initalize the function operations table
 */
void init_syscall(){
    /* stdin */
    stdin_fops[INDEX_0] = &terminal_read;
    stdin_fops[INDEX_1] = NULL;
    stdin_fops[INDEX_2] = &terminal_open;
    stdin_fops[INDEX_3] = &terminal_close;
    /* stdout */
    stdout_fops[INDEX_0] = NULL;
    stdout_fops[INDEX_1] = &terminal_write;
    stdout_fops[INDEX_2] = &terminal_open;
    stdout_fops[INDEX_3] = &terminal_close;
    /* rtc_fops */
    rtc_fops[INDEX_0] = &rtc_read;
    rtc_fops[INDEX_1] = &rtc_write;
    rtc_fops[INDEX_2] = &rtc_open;
    rtc_fops[INDEX_3] = &rtc_close;
    /* dir_fops */
    dir_fops[INDEX_0] = &dir_read;
    dir_fops[INDEX_1] = &dir_write;
    dir_fops[INDEX_2] = &dir_open;
    dir_fops[INDEX_3] = &dir_close;
    /* file_fops */
    file_fops[INDEX_0] = &file_read;
    file_fops[INDEX_1] = &file_write;
    file_fops[INDEX_2] = &file_open;
    file_fops[INDEX_3] = &file_close;

}
/* int32_t read(int32_t fd,void * buf,int32_t nbytes);
 * Inputs: int32_t fd,void * buf,int32_t nbytes
 * Return Value: return value from specific read operation
 * Function: based on fd, call specific read operation
 */
int32_t read(int32_t fd,void * buf,int32_t nbytes){
     if(fd==1){
        return FAIL;
    }
    if(buf==NULL){
        return FAIL;
    }
    /* get current pcb */
    pcb_t* pcb = get_pcb();
    
    /* condition check */
    if( fd < 0 || fd > MAXFILE || nbytes < 0){
        return FAIL;
    }
    /* Check if file is opened */
    if(pcb->file_array[fd].flag ==0){
        return FAIL;
    }
    /* call specific read operation */
    return pcb->file_array[fd].f_ops[0](fd,(char*)buf,nbytes);
   

}

/* int32_t write(int32_t fd,void * buf,int32_t nbytes);
 * Inputs: int32_t fd,void * buf,int32_t nbytes
 * Return Value: return value from specific write operation
 * Function: based on fd, call specific write operation
 */
int32_t write(int32_t fd,const void * buf,int32_t nbytes){
    if(buf==NULL){
        return FAIL;
    }
    if(fd==0){
        return FAIL;
    }
    /* get current pcb */
    pcb_t* pcb = get_pcb();
    /* condition check */
    if(fd < 0 || fd > MAXFILE || nbytes < 0){
        return FAIL;
    }
    /* Check if file is opened */
    if(pcb->file_array[fd].flag ==0){
        return FAIL;
    }
    
    /* call specific write operation */
    return pcb->file_array[fd].f_ops[1](fd,(char*)buf,nbytes);

}
/* int32_t close(int32_t fd);
 * Inputs: int32_t fd
 * Return Value: return value from specific close operation
 * Function: based on fd, call specific close operation
 */
int32_t close(int32_t fd){
    /* get current pcb */
   pcb_t* pcb = get_pcb();
     /* condition check */
    if(fd < USABLE_FD || fd > MAXFILE){
        return FAIL;
    }
    /* Check if file is opened */
    if(pcb->file_array[fd].flag ==0){
        return FAIL;
    }
    /* Set file as closed */
    pcb->file_array[fd].flag =0;
    /* call specific close operation */
    return pcb->file_array[fd].f_ops[INDEX_3](fd);
}
/* int32_t open(const uint8_t* filename);
 * Inputs: int32_t fd
 * Return Value: return value from specific open operation
 * Function: based on fd, call specific open operation
 */
int32_t open (const uint8_t* filename){
    /* Initialize fd */
    int32_t fd = DAFAULT_FD;
    dentry_t cur_dentry;
    int i;
    /* get current pcb */
   pcb_t* pcb = get_pcb();
    /* filename is null */
    if(filename==NULL){
        return FAIL;
    }
    
    if(filename==(uint8_t*)"stdin"){
        fd = 0;
        pcb->file_array[fd].f_ops[INDEX_0] = stdin_fops[INDEX_0];
        pcb->file_array[fd].f_ops[INDEX_1] = stdin_fops[INDEX_1];
        pcb->file_array[fd].f_ops[INDEX_2] = stdin_fops[INDEX_2];
        pcb->file_array[fd].f_ops[INDEX_3] = stdin_fops[INDEX_3];
        pcb->file_array[fd].inode = 0;
        pcb->file_array[fd].file_pos = 0;
        pcb->file_array[fd].flag = 1;
        return fd;
    }
    if(filename==(uint8_t*)"stdout"){
        fd = 1;
        pcb->file_array[fd].f_ops[INDEX_0] = stdout_fops[INDEX_0];
        pcb->file_array[fd].f_ops[INDEX_1] = stdout_fops[INDEX_1];
        pcb->file_array[fd].f_ops[INDEX_2] = stdout_fops[INDEX_2];
        pcb->file_array[fd].f_ops[INDEX_3] = stdout_fops[INDEX_3];
        pcb->file_array[fd].inode = 0;
        pcb->file_array[fd].file_pos = 0;
        pcb->file_array[fd].flag = 1;
        return fd;
    }
    /* find a available fd */
    for(i=USABLE_FD;i<MAXFILE;i++){
        if(pcb->file_array[i].flag ==0){
            fd = i;
            break;
        }
    }
    /* Can't find a fd, return */
    if(fd == DAFAULT_FD){
        return FAIL;
    }
    /* Can't find a dentry */
    if(read_dentry_by_name(filename,&cur_dentry)==-1){
        return FAIL;
    }
    
    /* Decide file operation depended on file type */
    switch(cur_dentry.filetype){
        /* rtc */
        case RTC:
            if(rtc_open(filename)!=0){
                return FAIL;
            }
            pcb->file_array[fd].f_ops[INDEX_0] = rtc_fops[INDEX_0];
            pcb->file_array[fd].f_ops[INDEX_1] = rtc_fops[INDEX_1];
            pcb->file_array[fd].f_ops[INDEX_2] = rtc_fops[INDEX_2];
            pcb->file_array[fd].f_ops[INDEX_3] = rtc_fops[INDEX_3];
            pcb->file_array[fd].inode = 0;
            pcb->file_array[fd].file_pos = 0;
            pcb->file_array[fd].flag = 1;
            return fd;
        /* directory */
        case DIR:
            if(dir_open(filename)!=0){
                return FAIL;
            }
            pcb->file_array[fd].f_ops[INDEX_0] = dir_fops[INDEX_0];
            pcb->file_array[fd].f_ops[INDEX_1] = dir_fops[INDEX_1];
            pcb->file_array[fd].f_ops[INDEX_2] = dir_fops[INDEX_2];
            pcb->file_array[fd].f_ops[INDEX_3] = dir_fops[INDEX_3];
            pcb->file_array[fd].inode = 0;
            pcb->file_array[fd].file_pos = 0;
            pcb->file_array[fd].flag = 1;
            return fd;
        /* file */
        case FILE:
            if(file_open(filename)!=0){
                return FAIL;
            }
            pcb->file_array[fd].f_ops[INDEX_0] = file_fops[INDEX_0];
            pcb->file_array[fd].f_ops[INDEX_1] = file_fops[INDEX_1];
            pcb->file_array[fd].f_ops[INDEX_2] = file_fops[INDEX_2];
            pcb->file_array[fd].f_ops[INDEX_3] = file_fops[INDEX_3];
            pcb->file_array[fd].inode = cur_dentry.inode_num;
            pcb->file_array[fd].file_pos = 0;
            pcb->file_array[fd].flag = 1;
            return fd;
        default:
            return FAIL;
    }
   

}

/* int32_t halt(uint8_t status);
 * Inputs: uint8_t status
 * Return Value: -1
 * Function: halt the current program, after the halt, it return back to its parent
 */
int32_t halt (uint8_t status){
    int i;
    
    /* get current pcb */
    pcb_t* pcb = get_pcb();
    /* restore esp0 and ss0 */
    tss.esp0 = pcb->esp0;
    tss.ss0 = KERNEL_DS;
    /* close files */
    for(i=USABLE_FD;i<MAXFILE;i++){
        pcb->file_array[i].flag=0;
    }
    // /*  */
    // if(pcb->parent_pcb==NULL){
    //     execute((uint8_t*)"shell");
    // }
    pcb->pid = pcb->parent_pid;
    pcb->status_halt = status;
    
    /* restore paging */
    get_addr(pcb->pid);
   


    /* return to the stored address */
    asm volatile("          \n\
        movl %0,%%ebp       \n\
        movl %1,%%esp       \n\
        jmp Return_iret          \n\
        "
        :
        :"r"(pcb->parent_ebp),"r"(pcb->parent_esp)
    );
    
    return -1;
}

/* int32_t execute (const uint8_t* command)
 * Inputs: const uint8_t* command
 * Return Value: pcb return address
 * Function: execute the file, first we parse the command, next check whether it is executable
 * or not. Then, we set the paging and load the file.
 * create PCB and do context switch
 */

int32_t execute (const uint8_t* command){
    uint32_t ret_addr;
    uint32_t parent_esp;
    uint32_t parent_ebp;
    //condition check
    if (command == NULL)
    {
        return -1;
    }
    //initialization
    uint8_t file_name[NUM_32];
    uint8_t argument[NUM_128];
    
    int location;
    memset(arg_buf,0,NUM_128);
   //parsing
    if(parse(file_name, argument, command) == -1){
        return -1;
    }
    //check whether it is a executable file
    if(check_executable(file_name) == -1){
        return -1;
    }
    //get the first available process
    location = get_first_available_index();
    if(location == -1){
        printf("Too many processes. \n");
        return -1;
    }
  
    pcb_t * pcb = get_pcb();
    pcb->pid = location;
    pcb->parent_pid = parent_pid;
    parent_pid = pcb->pid;

    //paging
    get_addr(location);
    
    
    
    //loading the program
    if(program_loader(location) == -1){
        return -1;
    }

    //create the pcb
    create_pcb(pcb);
    pcb->esp0 = tss.esp0;
    

    asm volatile("           \n\
        movl %%esp,%0         \n\
        movl %%ebp,%1"
        :"=r"(parent_esp),"=r"(parent_ebp)
        :
        :"memory"
    );
    pcb->parent_ebp = parent_ebp;
    pcb->parent_esp = parent_esp;

   
    pcb->ret_addr = ret_addr;

    open((uint8_t*)"stdin");
    open((uint8_t*)"stdout");
    //context_switch

    context_switch(pcb);

   
    return pcb->ret_addr;
}


/* int32_t getargs(void * buf,int32_t nbytes);
 * Inputs: void * buf,int32_t nbytes
 * Return Value: 0
 * Function: get the argument read from the command
 */
int32_t getargs (void * buf,int32_t nbytes){
    //error check
    if(buf == NULL || nbytes < 0 || buf == "")
        return -1;
  
    //copy args to buf
    memcpy(buf, arg_buf, NUM_128); //argument buffer in side pcb or just local

    return 0;
}
/* int32_t vidmap(uint8_t** screen_start);
 * Inputs: uint8_t** screen_start
 * Return Value: 0
 * Function: videp memory mapping
 */
int32_t vidmap (uint8_t** screen_start){
    //error checking
    if (screen_start == NULL)
        return -1;
    //128~132MB
    if ((int)screen_start < ADDR_128MB || (int)screen_start >= ADDR_132MB)
        return -1;
    //132MB->use this as virtual address
    //set up paging such that the virtual address refers to physical vedio memory
    vidmap_paging(ADDR_132MB,PHYS_MEM);
    //*screen_start = 132MB
    *screen_start = (uint8_t*)ADDR_132MB;
    flush_tlb();
    //return
    return 0;
}
/* int32_t set_handler(int32_t signum,void * handler_address);
 * Inputs: int32_t signum,void * handler_address
 * Return Value: 0
 * Function: optinal function for chekcpoint 3.4
 */
int32_t set_handler(int32_t signum,void * handler_address){
    return 0;
}
/* int32_t sigreturn(void);
 * Inputs: void
 * Return Value: 0
 * Function: optinal function for chekcpoint 3.4
 */
int32_t sigreturn(void){
    return 0;
}

/* int32_t get_pcb(void);
 * Inputs: void
 * Return Value: current pcb pointer
 * Function: get current pcb pointer
 */
pcb_t* get_pcb(){
    asm volatile("movl %%esp,%0":"=g"(pcb_esp));
    pcb_esp = pcb_esp >> MASK_13;
    pcb_esp = pcb_esp << MASK_13;
    return (pcb_t*)pcb_esp;

};

/* int32_t create_pcb(pcb_t* cur,int par_pid);
 * Inputs: pcb_t* cur,int par_pid
 * Return Value: create pcb
 * Function: set the function operations table for pcb
 */
void create_pcb(pcb_t* cur){
    int i;
    
    /* setup stdin and stdout */
    for(i=0;i<2;i++){
        if(i==0){
        cur->file_array[i].f_ops[INDEX_0] = stdin_fops[INDEX_0];
        cur->file_array[i].f_ops[INDEX_1] = stdin_fops[INDEX_1];
        cur->file_array[i].f_ops[INDEX_2] = stdin_fops[INDEX_2];
        cur->file_array[i].f_ops[INDEX_3] = stdin_fops[INDEX_3];
        }
        else{
            cur->file_array[i].f_ops[INDEX_0] = stdout_fops[INDEX_0];
            cur->file_array[i].f_ops[INDEX_1] = stdout_fops[INDEX_1];
            cur->file_array[i].f_ops[INDEX_2] = stdout_fops[INDEX_2];
            cur->file_array[i].f_ops[INDEX_3] = stdout_fops[INDEX_3];
        }
        cur->file_array[i].inode=0;
        cur->file_array[i].file_pos=0;
        cur->file_array[i].flag=1;
    }
    /* store esp0 and parent pid */
    
}

/* int32_t parse(uint8_t* file_name,uint8_t* argument, const uint8_t* command)
 * Inputs: uint8_t* file_name,uint8_t* argument, const uint8_t* command
 * Return Value: 0, -1
 * Function: parse the command, get the filename and the argument
 */
int32_t parse(uint8_t* file_name,uint8_t* argument, const uint8_t* command){
    //initialization
    int i;
    int space_flag = 0;
    memset(argument, 0, NUM_128);
    memset(file_name, 0 , NUM_32);
    int space_index = 0;
    //parse the filename and argument from the command
    for(i = 0; command[i] != '\0'; i++)
    {
         //check if space is reached
        if (command[i] == ' ')
        {
            space_flag = 1;
            space_index = i;
            continue;
        }
        //truncate name
        if ( (i > NUM_32 - 1) && !space_flag)
        {
            break;
        }
        //copy the file name
        
        if (!space_flag){
            if(i - space_index > (NUM_32 - 1))
            {
                break;
            }
            file_name[i] = command[i];
        }
        //copy the argument
        else
        {
            argument[i - space_index - 1] = command[i];
            arg_buf[i - space_index - 1] = command[i];
        }
        
    }

    return 0;
}

/* check_executable(uint8_t* file_name)
 * Inputs: uint8_t* file_name
 * Return Value: 0, -1
 * Function: check whether the file is executable
 */
int32_t check_executable(uint8_t* file_name){
    //condition check
    if(file_name == NULL)
    {
        return -1;
    }
    // printf(file_name);
    // printf("qqq");
    if(read_dentry_by_name(file_name, &dentry) == -1)
    {
        return -1;
    }
    //initialization
    uint8_t check_buf[SIZE_4];

    //check whether the it is readable
    if(read_data(dentry.inode_num, 0, check_buf, SIZE_4) == -1)
    {
        return -1;
    }
    if(check_buf[INDEX_0] != MAGIC_1 || check_buf[INDEX_1] != MAGIC_2 || check_buf[INDEX_2] != MAGIC_3 || check_buf[INDEX_3] != MAGIC_4){
        return -1;
    }
    return 0;
}
/* get_first_available_index()
 * Inputs: none
 * Return Value: i, -i
 * Function: get next available pid for the current program
 */
int32_t get_first_available_index(){
    int i = 0;
    while (process[i] != 0){
        i += 1;
    }
    process[i]=1;
    if(i >= SIZE_6)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

/* get_addr(int location)
 * Inputs: int locationne
 * Return Value: none
 * Function: get starting address for current index
 */
void get_addr(int location){
   
    load_addr = MB8;
    load_addr += location*MB4;
   
    execute_paging(load_addr);
    // disable_paging(load_addr);
    flush_tlb();
    // enable_paging(load_addr);
}

/* program_loader()
 * Inputs: none
 * Return Value: 0, -1
 * Function: function which load the program 
 */
int32_t program_loader(int location){
    inode_t* inodes[boot_block->inode_count];
    int i;
    for(i = 0; i < boot_block->inode_count; i++){
        inodes[i] = (inode_t*)((int)boot_block + B_SIZE * (i + 1));
    }
    if (read_data(dentry.inode_num,0,(uint8_t*)(STARTADDR),inodes[dentry.inode_num]->length) == -1){
        return -1;
    }

    // if (read_data(dentry.inode_num,0,(uint8_t*)(load_addr + MAGIC),inodes[dentry.inode_num]->length) == -1){
    //     return -1;
    // }
    return 0;
}
/* flush_tlb()
 * Inputs: none
 * Return Value: none
 * Function: flush the tlb
 */
void flush_tlb(){
    asm volatile ("     \n\
    movl %%cr3, %%eax     \n\
    movl %%eax, %%cr3     \n\
    "
    :
    :
    :"eax"
    );
}

/* context_switch(pcb_t* pcb)
 * Inputs: none
 * Return Value: none
 * Function: set tss, prepare iret context and call IRET
 */
void context_switch(pcb_t* pcb){
    tss.ss0 = KERNEL_DS;
    tss.esp = (MB8 - (KB8 * pcb->pid) - SIZE_4);

    asm volatile(
     
    "movl $0x2B, %%ecx;"
    "pushl %%ecx;"
    "movl $0x83FFFFC, %%ecx;"
    "pushl %%ecx;"
    "pushfl;"
    "popl %%ecx;"  
    "orl $0x200, %%ecx;"   
    "pushl %%ecx;"
    "movl $0x0023, %%ecx;"
    "pushl %%ecx;"
    "xorl %%ecx, %%ecx;"
    "movl $0x08048018, %%eax;"
    "movl (%%eax),%%eax;"
    "addl %%eax,%%ecx;"
    "pushl %%ecx;"
    
    "iret;"
    "Return_iret:;"
    "leave;"
    "ret;"
   

    :
    :
    : "eax","ecx","memory"
    );
}

// "i"(USER_DS),"i"(USER_ESP),"r"(0x200),"i"(USER_CS),"r"(eip)


