#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "int_handler.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0


// static int t_read_test();
// static int rtc_test();

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

// static int Divide_zero_test();
// static int page_fault_test();
// static int page_not_fault_test();

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	
	clear();
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
/* 
 * Divide_zero_test
 * Divide by 0 test to get exception
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Divided by 0 exception
 * Files: idt.h/c
 */
// int Divide_zero_test(){
// 	int a = 1;
// 	int b = 0;
// 	TEST_HEADER;

// 	int i;
// 	i = a/b;
// 	return 1;
// }

// // add more tests here
/* RTC_print_test
 * 
 * Print a line to test RTC
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: RTC interrupt
 * Files: int_handler.h/c
 */
// int RTC_print_test(){
// 	TEST_HEADER;
// 	printf("test_RTC");
// 	return 1;
// }
/* page_fault_test
 * 
 * Dereference a null pointer
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: invalid paging
 * Files: paging.h/c
 */
// int page_fault_test(){
// 	TEST_HEADER;
// 	int* a = 0x0;
// 	int b;
// 	b = *a;
// 	return 1;
// }
/* page_not_fault_test
 * 
 * Dereference a valid pointer
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: valid paging
 * Files: paging.h/c
 */
// int page_not_fault_test(){
// 	TEST_HEADER;
// 	int a = 0x1;
// 	int* b;
// 	b = &a;
// 	return 1;
// }

/* Checkpoint 2 tests */
int t_read_test(){
	char buf[128];
	int n;
	

	printf("Hello, please tell me your name:");
	while(1){
		memset(buf,0,128);
		n = terminal_read(0,buf,3);
		if(n>0){
			break;
		}
		
	}
	printf("Read succeed %d\n",n);
	printf("Your name is: ");
	n = terminal_write(1,buf,3);
	
	return 0;
}

int rtc_test(){
	int i;
    int j;
    int freq =1;
    for(i = 1;i<11;i++){
        clear();
        freq *= 2;
        rtc_write(0,&freq,4);
        for(j=0;j<20;j++){
            rtc_read(0,&freq,4);
        }
    }
	rtc_open(0);
	return 0;
}

/* Checkpoint 3 tests */

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	t_read_test();
	
	// rtc_test();
	
	// TEST_OUTPUT("Divide_zero_test", Divide_zero_test());
	// TEST_OUTPUT("page_fault_test", page_fault_test());
	// TEST_OUTPUT("page_not_fault_test", page_not_fault_test());
	
	// launch your tests here
}
