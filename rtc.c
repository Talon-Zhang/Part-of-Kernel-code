#include "rtc.h"
#include "i8259.h"
#include "lib.h"


volatile int interrupt_flag;

/* void init_rtc(void);
 * Inputs: void
 * Return Value: none
 * Function: initialize RTC
 * Reference: https: //wiki.osdev.org/RTC
 */
void init_rtc(void){

	//select register B, and disable NMT
    outb(REGISTER_B, RTC_PORT);
    //read the current value of register B
    uint8_t b = inb(CMOS_PORT);
    //set the index again
    outb(REGISTER_B, RTC_PORT);
    //write the previous value ORed with 0x40
    outb(b | mask_40, CMOS_PORT);
    //enable IRQ
    enable_irq(RTC_IRQ);  
    //select register A, and disable NMT
    outb(REGISTER_A, RTC_PORT);
    //read the current value of register A
    uint8_t a = inb(CMOS_PORT);
    //set the index again
    outb(REGISTER_A, RTC_PORT);
    //set the frequency
    outb((a & mask_F0) | rate,CMOS_PORT);

}




/* void change_flag(void);
 * Inputs: void
 * Return Value: none
 * Function:  change interrupt_flag
 * 
 */
void change_flag(){
    interrupt_flag = 1;
}

/* int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: 0
 * Function: read function for rtc, must be synchronized, such that
 * a new read occured after the last interrupt ends
 * Reference: https: //wiki.osdev.org/RTC
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
	//while the interrupt flag is 0
	//spin until the interrupt occurred
    interrupt_flag = 0;
	sti();
	while(interrupt_flag == 0);
	//reset the flag
	interrupt_flag = 0;
	//return 0
	return 0;
}

/* rtc_write(int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: int32_t fd, const void* buf, int32_t nbytes
 * Return Value: nbytes
 * Function: write function for rtc, set the frequency of the rtc
 * Reference: https: //wiki.osdev.org/RTC
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){

	uint8_t rate_local;
	//the number of bytes write must be 4
	if(nbytes != NUM_OF_BYTES){
		//if not 4, return -1
		return -1;
	}
	//for each rate of RTC, find the corresponding
	//frequency to set for RTC
	//the range of rate should be in 2 to 1024
	if(*(int32_t*)buf == Hz_2){
		rate_local = F_2;
	}
	else if(*(int32_t*)buf == Hz_4){
		rate_local = F_4;
	}
	else if(*(int32_t*)buf == Hz_8){
		rate_local = F_8;
	}
	else if(*(int32_t*)buf == Hz_16){
		rate_local = F_16;
	}
	else if(*(int32_t*)buf == Hz_32){
		rate_local = F_32;
	}
	else if(*(int32_t*)buf == Hz_64){
		rate_local = F_64;
	}
	else if(*(int32_t*)buf == Hz_128){
		rate_local = F_128;
	}
	else if(*(int32_t*)buf == Hz_256){
		rate_local = F_256;
	}
	else if(*(int32_t*)buf == Hz_512){
		rate_local = F_512;
	}
	else if(*(int32_t*)buf == Hz_1024){
		rate_local = F_1024;
	}else{
		//if rate is not in the coreect range
		//return -1
		return -1;
	}

	//set the new frequency
	outb(REGISTER_A,RTC_PORT);
	uint8_t a = inb(CMOS_PORT);
	outb(REGISTER_A,RTC_PORT);
	outb((a & mask_F0) | rate_local, CMOS_PORT);

	//return the number of bytes writen
	return nbytes;

}

/* rtc_open(const unit8_t * filename)
 * Inputs: const unit8_t * filename
 * Return Value: 0
 * Function: open function for rtc
 * Reference: https: //wiki.osdev.org/RTC
 */
int32_t rtc_open(const uint8_t * filename){
	init_rtc();
	return 0;
}

/* int32_t rtc_close(int32_t fd)
 * Inputs: int32_t fd
 * Return Value: 0
 * Function: close function for rtc
 * Reference: https: //wiki.osdev.org/RTC
 */
int32_t rtc_close(int32_t fd){
	disable_irq(RTC_IRQ);
	return 0;
}
