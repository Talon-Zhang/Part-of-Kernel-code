/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define SLAVE_IRQ 2

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* void i8259_init(void)
* input: none
* retun: none
* Initialize the 8259 PIC 
* reference: https://wiki.osdev.org/8259_PIC*/
void i8259_init(void) {

    //set mask
    master_mask = mask_FF;
    slave_mask = mask_FF;
    outb(mask_FF, MASTER_8259_PORT + 1);
    outb(mask_FF, SLAVE_8259_PORT + 1);

    outb(ICW1, MASTER_8259_PORT);                    // ICW1: Master
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);     // ICW2: Master
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);         // ICW3: Master
    outb(ICW4, MASTER_8259_PORT + 1);                // ICW4: Master
    outb(ICW1, SLAVE_8259_PORT);                     // ICW1: Slave
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);           // ICW2: Slave
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);           // ICW3: Slave
    outb(ICW4, SLAVE_8259_PORT + 1);                 // ICW4: Slave

    outb(master_mask, MASTER_8259_PORT + 1);         //restore IRQ of master
    outb(slave_mask, SLAVE_8259_PORT + 1);           //restore IRQ of slave

    enable_irq(SLAVE_IRQ);                           // enable IRQ
}

/* void enable_irq(uint32_t irq_num)
* input: uint32_t irq_num
* return: none
* set the mask for irq
*/
void enable_irq(uint32_t irq_num) {
    uint32_t mask;
    //for irq of master
    if((irq_num >=0) & (irq_num < IRQ_8)){
        //mask for that irq
        mask = ~(1 << (irq_num));
        master_mask &= mask;
        //set the mask
        outb(master_mask,MASTER_8259_PORT + 1);
    }
    //for irq of slave
    else if((irq_num >=IRQ_8) & (irq_num<=IRQ_15)){
        //mask for that irq
        mask = ~(1 << (irq_num - IRQ_8));
        slave_mask &= mask;
        //set the mask
        outb(slave_mask,SLAVE_8259_PORT + 1);
    }
    else{
        return;
    }
}

/* void disable_irq(uint32_t irq_num)
* input: uint32_t irq_num
* return: none
* clear the mask for irq
*/
void disable_irq(uint32_t irq_num) {
    
    uint32_t mask;
    //for irq of master
    if((irq_num >=0) & (irq_num < IRQ_8)){
        //mask for that irq
        mask = 1 << (irq_num);
        master_mask |= mask;
        //clear the mask
        outb(master_mask,MASTER_8259_PORT + 1);
    }
    //for irq of slave
    else if((irq_num >=IRQ_8) & (irq_num<=IRQ_15)){
        //mask for that irq
        mask = 1 << (irq_num - IRQ_8);
        slave_mask |= mask;
        //clear the mask
        outb(slave_mask,SLAVE_8259_PORT + 1);
    }
    else{
        return;
    }
}

/* void send_eoi(uint32_t irq_num) 
* input: uint32_t irq_num
* return: none
* Send end-of-interrupt signal for the specified IRQ 
*/

void send_eoi(uint32_t irq_num) {
    //for irq of master
    if((irq_num >=0) & (irq_num < IRQ_8)){
        //end-of-interrupt signal
        outb(EOI|irq_num,MASTER_8259_PORT);
    }
    else if((irq_num >=IRQ_8) & (irq_num<=IRQ_15)){
        //end-of-interrupt signal for slave
        outb(EOI|(irq_num - IRQ_8),SLAVE_8259_PORT);
        //end-of-interrupt signal also for master
        outb(EOI|SLAVE_IRQ,MASTER_8259_PORT);
    }
    else{
        return;
    }
}
