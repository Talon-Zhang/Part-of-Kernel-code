.text

.global SYS_linkage

# void SYS_linkage();
#   Inputs: None
#   Return Value: None
#    Function: Saveregisters and call handler for system call*/
SYS_linkage:
    pushf 
    # save registers
    pushl %ebp
    pushl %edi
    pushl %esi
    
    # calculate index and check if it's in range (1<%eax<10)
    cmpl $1,%eax
    jl invalid
    cmpl $10,%eax
    jg invalid 
    # decrease by 1 for jumtable
    addl $-1,%eax

    pushl %edx
    pushl %ecx
    pushl %ebx
    
    # go to jump_table, the increment is 4.
    call *jump_table(,%eax,4)

    popl %ebx
    popl %ecx
    popl %edx

    jmp Done
jump_table:
    .long halt,execute,read,write,open,close,getargs,vidmap,set_handler,sigreturn

Done:   
    popl %esi
    popl %edi
    popl %ebp
   
    popf

    iret


invalid:
    # return -1 if failed
    movl $-1,%eax
    jmp Done
