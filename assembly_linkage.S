.text

.global RTC_int_linkage, keyboard_int_linkage 

# void RTC_int_linkage();
#   Inputs: None
#   Return Value: None
#    Function: Save all registers and call handler for RTC*/
RTC_int_linkage:
    pushf 
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx

    call RTC_int

    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    popf

    iret

#  void keyboard_int_linkage();
#   Inputs: None
#   Return Value: None
#    Function: Save all registers and call handler for KEYBOARD*/
keyboard_int_linkage:
    pushf 
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx

    call keyboard_int

    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    popf

    iret
