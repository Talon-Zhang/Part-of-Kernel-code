#include "idt.h"
#include "x86_desc.h"
#include "assembly_linkage.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "SYS_assembly_linkage.h"

/* Constant */
#define RESERVED_NUM 0x20
#define KEYBOARD_VEC 0X21
#define RTC_VEC 0X28
#define SYS_CALL_VEC 0X80

#define USER_LEVEL_DPL 3

#define DIVIDE_ERROR_VEC 0x00
#define RESERVED_VEC 0X01
#define NMI_Interrupt_VEC 0X02
#define BREAKPOINT_VEC 0X03
#define OVERFLOW_VEC 0X04
#define BOUND_VEC 0X05
#define INVALID_OPCODE_VEC 0X06
#define DEVICE_VEC 0X07
#define DOUBLE_FAULT_VEC 0X08
#define OVERRUN_VEC 0X09
#define TSS_VEC 0X0A
#define SEGMENT_NOT_PRESENT_VEC 0X0B
#define Stack_Segment_Fault_VEC 0X0C
#define GENERAL_PROTECTION_VEC 0X0D
#define PAGE_FAULT_VEC 0X0E
#define FLOATING_POINT_ERROR_VEC 0X0F
#define Alignment_Check_VEC 0X10
#define Machine_Check_VEC 0X11
#define Floating_Point_Exception_VEC 0X12
/* Local helper function for exception and system call */
static void divide_error();
static void RESERVED();
static void NMI_Interrupt();
static void Breakpoint();
static void Overflow();
static void BOUND_Range_Exceeded();
static void Invalid_Opcode();
static void Device_Not_Available();
static void Double_Fault();
static void Coprocessor_Segment_Overrun();
static void Invalid_TSS();
static void  Segment_Not_Present();
static void Stack_Segment_Fault();
static void General_Protection();
static void Page_Fault();
static void Floating_Point_Error();
static void Alignment_Check();
static void Machine_Check();
static void Floating_Point_Exception();
// static void SYS_CALL();
static void fill_idt();


/* void divide_error();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: divide_error*/
static void divide_error(){
    printf("divide_error");
    /* stop exception*/
    while(1);
}
/* void RESERVED();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: RESERVED*/
static void RESERVED(){
    printf("RESERVED");
    while(1);
}
/* void NMI_Interrupt();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: NMI_Interrupt*/
static void NMI_Interrupt(){
    printf("NMI_Interrupt");
    while(1);
}
/* void Breakpoint();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Breakpoint*/
static void Breakpoint(){
    printf("Breakpoint");
    while(1);
}
/* void Overflow();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Overflow*/
static void Overflow(){
    printf("Overflow");
    while(1);
}
/* void BOUND_Range_Exceeded();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: BOUND_Range_Exceeded*/
static void BOUND_Range_Exceeded(){
    printf("BOUND Range Exceeded");
    while(1);
}
/* void Invalid_Opcode();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Invalid_Opcode*/
static void Invalid_Opcode(){
    printf("Invalid Opcode (Undefined Opcode)");
    while(1);
}
/* void Device_Not_Available();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Device_Not_Available*/
static void Device_Not_Available(){
    printf("Device Not Available (No Math Coprocessor)");
    while(1);
}
/* void Double_Fault();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Double_Fault*/
static void Double_Fault(){
    printf("Double Fault");
    while(1);
}
/* void Coprocessor_Segment_Overrun();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Coprocessor_Segment_Overrun*/
static void Coprocessor_Segment_Overrun(){
    printf("Coprocessor Segment Overrun (reserved)");
    while(1);
}
/* void Invalid_TSS();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Invalid_TSS*/
static void Invalid_TSS(){
    printf("Invalid TSS");
    while(1);
}
/* void Segment_Not_Present();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Segment_Not_Present*/
static void  Segment_Not_Present(){
    printf("Segment Not Present");
    while(1);
}
/* void RESERVED();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print exception: RESERVED*/
static void Stack_Segment_Fault(){
    printf("Stack-Segment Fault");
    while(1);
}
/* void General_Protection();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: General_Protection*/
static void General_Protection(){
    printf("General Protection");
    while(1);
}
/* void Page_Fault();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Page_Fault*/
static void Page_Fault(){
    printf("Page Fault");
    while(1);
}
/* void Floating_Point_Error();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Floating_Point_Error*/
static void Floating_Point_Error(){
    printf("x87 FPU Floating-Point Error (Math Fault)");
    while(1);
}
/* void Alignment_Check();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Alignment_Check*/
static void Alignment_Check(){
    printf("Alignment Check");
    while(1);
}
/* void Machine_Check();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Machine_Check*/
static void Machine_Check(){
    printf("Machine Check");
    while(1);
}
/* void Floating_Point_Exception();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: Floating_Point_Exception*/
static void Floating_Point_Exception(){
    printf("SIMD Floating-Point Exception");
    while(1);
}
/* void SYS_CALL();
 *   Inputs: None
 *   Return Value: None
 *    Function: Print: system calls*/
// static void SYS_CALL(){
//     printf("system calls");
//     return;
    
// }

/* void fill_idt();
 *   Inputs: None
 *   Return Value: None
 *    Function: Fill IDT table. Note that there is a subtle difference between
 *    the first 32 elements and the others*/
static void fill_idt(){
    int i = 0;  //Loop index
    /* Set reserved 3 = 1 for The first 32 reserved elements */ 
    for(i=0;i<RESERVED_NUM ;i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 1;
    }
    /* set reserved 3 = 0 for the other elements */
    for(i=RESERVED_NUM;i<NUM_VEC;i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 0;
        
    }
    /* DPL for system call is 3 */

    idt[KEYBOARD_VEC].present = 1;
    idt[RTC_VEC].present = 1;
    idt[SYS_CALL_VEC].present = 1;
    idt[SYS_CALL_VEC].dpl = USER_LEVEL_DPL;
}

/* void init_idt();
 *   Inputs: None
 *   Return Value: None
 *    Function: FILL IDT and set entries of IDT*/

void init_idt(){
    fill_idt();
    /* Set the first 20 default exceptions */
    SET_IDT_ENTRY(idt[DIVIDE_ERROR_VEC],divide_error);
    SET_IDT_ENTRY(idt[RESERVED_VEC],RESERVED);
    SET_IDT_ENTRY(idt[NMI_Interrupt_VEC],NMI_Interrupt);
    SET_IDT_ENTRY(idt[BREAKPOINT_VEC],Breakpoint);
    SET_IDT_ENTRY(idt[OVERFLOW_VEC],Overflow);
    SET_IDT_ENTRY(idt[BOUND_VEC],BOUND_Range_Exceeded);
    SET_IDT_ENTRY(idt[INVALID_OPCODE_VEC],Invalid_Opcode);
    SET_IDT_ENTRY(idt[DEVICE_VEC],Device_Not_Available);
    SET_IDT_ENTRY(idt[DOUBLE_FAULT_VEC],Double_Fault);
    SET_IDT_ENTRY(idt[OVERRUN_VEC],Coprocessor_Segment_Overrun);
    SET_IDT_ENTRY(idt[TSS_VEC],Invalid_TSS);
    SET_IDT_ENTRY(idt[SEGMENT_NOT_PRESENT_VEC],Segment_Not_Present);
    SET_IDT_ENTRY(idt[Stack_Segment_Fault_VEC],Stack_Segment_Fault);
    SET_IDT_ENTRY(idt[GENERAL_PROTECTION_VEC],General_Protection);
    SET_IDT_ENTRY(idt[PAGE_FAULT_VEC],Page_Fault);
    SET_IDT_ENTRY(idt[FLOATING_POINT_ERROR_VEC],Floating_Point_Error);
    SET_IDT_ENTRY(idt[Alignment_Check_VEC], Alignment_Check);
    SET_IDT_ENTRY(idt[Machine_Check_VEC],Machine_Check);
    SET_IDT_ENTRY(idt[Floating_Point_Exception_VEC],Floating_Point_Exception);
    /* Set RTC entry */
    SET_IDT_ENTRY(idt[RTC_VEC],RTC_int_linkage);
    /* Set keyboard entry */
    SET_IDT_ENTRY(idt[KEYBOARD_VEC],keyboard_int_linkage);
    /* Set system call entry */
    SET_IDT_ENTRY(idt[SYS_CALL_VEC],SYS_linkage);

    lidt(idt_desc_ptr); 

}
