
#include "tests.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "rtc.h"

#define RTC_CMD_PORT 0x70
#define RTC_DATA_PORT 0x71
#define REG_C 0x0C
#define RTC_EOI 8
#define NUM_LETTER_SIZE 0x3B
#define KEYBOARD_DATA_PORT 0x60

#define BACKSPACE_PRESSED 0x0E
#define ENTER_PRESSED 0x1C
#define ENTER_RELEASED 0x9C
#define CTRL_PRESSED 0x1D
#define CTRL_RELEASED 0x9D
#define LEFT_SHIFT_PRESSED 0x2A
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_PRESSED 0x36
#define RIGHT_SHIFT_RELEASED 0xB6
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8
#define CAPSLOCK_PRESSED 0x3A
#define F1_PRESSED 0x3B
#define F2_PRESSED 0x3C
#define F3_PRESSED 0x3D
#define functional_shift 8

#define NUM_CHARACTER 0x3B 
#define LOWER_A 97
#define LOWER_Z 122
#define BUF_SIZE 128
#define NUM_COL 80

static int ctr_pressed = 0;
static int alt_pressed = 0;
static int cap_pressed = 0;
static int left_shift_pressed = 0;
static int right_shift_pressed = 0;
static volatile int enter_pressed = 0;
static char typed_buf[BUF_SIZE];
static char terminal_buf[BUF_SIZE];
static int cur_index = 0;
static int written_size = 0;

/* Array of numbers and lower case letters */
const uint8_t Lower_case[NUM_LETTER_SIZE] = 
 {  
	' ',' ','1', '2', '3', '4', '5', '6', '7', '8', '9', '0','-','=', '\b', ' ','q', 'w','e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',']',' ', 
    ' ', 'a', 's', 'd', 'f','g', 'h', 'j', 'k', 'l', ';', '\'', '`', ' ', '\\',  
    'z', 'x', 'c', 'v', 'b', 'n', 'm',',','.','/',' ',' ',' ',' ',' '
};
const uint8_t Upper_case[NUM_LETTER_SIZE] = 
 {  
	' ',' ','!', '@', '#', '$', '%', '^', '&', '*', '(', ')','_','+', ' ', ' ','Q', 'W','E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{','}',' ', 
    ' ', 'A', 'S', 'D', 'F','G', 'H', 'J', 'K', 'L', ':', '\"', '~', ' ', '|',  
    'Z', 'X', 'C', 'V', 'B', 'N', 'M','<','>','?',' ',' ',' ',' ',' '
};

/* void RTC_int();
 *   Inputs: None
 *   Return Value: None
 *    Function: Interrupt for RTC */
void RTC_int(){
    outb(REG_C,RTC_CMD_PORT);
    inb(RTC_DATA_PORT);
    
    // printf("1");
    change_flag();
    /* Send EOI for this interrupt */
    send_eoi(RTC_EOI);
}
/* void keyboard_int();
 *   Inputs: None
 *   Return Value: None
 *    Function: Interrupt for keyboard*/
void keyboard_int(){
    int flag_alphanumeric = 0;
    int i;
    /* Get input scan code */
    uint8_t pressed_key = inb(KEYBOARD_DATA_PORT);
    uint8_t key;
    
  
    /* Switch cases for functional keys such as control, shift and etc. Set to one if
    a key is pressed and to zero if it is released */
    switch(pressed_key){
        case CTRL_PRESSED:
            ctr_pressed = 1;
            break;
        case CTRL_RELEASED:
            ctr_pressed = 0;
            break;
        case LEFT_SHIFT_PRESSED:
            left_shift_pressed = 1;
            break;
        case LEFT_SHIFT_RELEASED:
            left_shift_pressed = 0;
            break;
        case RIGHT_SHIFT_PRESSED:
            right_shift_pressed = 1;
            break;
        case RIGHT_SHIFT_RELEASED:
            right_shift_pressed = 0;
            break;
        case ALT_PRESSED:
            alt_pressed = 1;
            break;
        case ALT_RELEASED:
            alt_pressed = 0;
            break;
        case CAPSLOCK_PRESSED:
            if(cap_pressed == 1){
                cap_pressed = 0;
            }
            else{
                cap_pressed = 1;
            }
            break;
        default:
            flag_alphanumeric = 1;
            break;
    }
    /* key pressed is not listed above */
    if(flag_alphanumeric){
    /* Find the corresponding key to print */
        if(pressed_key<NUM_CHARACTER){
            key = Lower_case[pressed_key];
            /* Backspace */
            if(pressed_key == BACKSPACE_PRESSED){
                /* only backspace at position greater than 0 */
                if(cur_index>0){
                    /* move index of buffer backward */
                    cur_index--;
                    typed_buf[cur_index] = key;
                    /* delete corresponding character */
                    putc(key);
                }
                send_eoi(1);
                return;
            }    
            /* Enter */
            if(pressed_key == ENTER_PRESSED){
                if(cur_index!=0){
                    for(i=0;i<BUF_SIZE;i++){
                        terminal_buf[i] = typed_buf[i];
                        written_size = cur_index;
                    } }
                enter_pressed = 1;
                /* change line */
                cur_index = 0;
                putc('\n');
                send_eoi(1);
                return;
            }
            if(pressed_key == ENTER_RELEASED){
                for(i=0;i<BUF_SIZE;i++){
                        typed_buf[i] = '\0';}
                cur_index = 0;
                enter_pressed = 0;
                send_eoi(1);
                return;
            }
            /* Control + L, clear the screen */
            if(key == 'l' && ctr_pressed == 1){
                cur_index = 0;
                clear();
                send_eoi(1);
                return;
            }
            //Capslock pressed
            if(cap_pressed == 1){
                /* Lower case a to z */
                if((int)key >= LOWER_A && (int)key <= LOWER_Z){
                    /* switch to upper case */
                    key = Upper_case[pressed_key];
                }
            }
            /* shift pressed. */
            if(left_shift_pressed || right_shift_pressed){
                if(cap_pressed == 1){
                    key = Lower_case[pressed_key];
                }
                else{
                    key = Upper_case[pressed_key];}
            }
            /* We have reached the limitation of buffer */
            if(cur_index==BUF_SIZE-1){
                send_eoi(1);
                return;
            }
            /* Add typed key to buffer and print it */
            typed_buf[cur_index] = key;
            cur_index++;
            putc(key);
            /* Change line */
            
        }
    }
   
   

    /* send EOI */
    send_eoi(1);
    return;


}

/* int32_t terminal_open();
 *   Inputs: None
 *   Return Value: 0;
 *    Function: Open terminal*/
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/* int32_t terminal_close();
 *   Inputs: None
 *   Return Value: 0;
 *    Function: Open terminal*/
int32_t terminal_close(int32_t fd){
    return 0;
}


/* int32_t terminal_read();
 *   Inputs: int32_t fd,void * buf, int32_t nbytes
 *   Return Value: number of bytes read from terminal
 *    Function: read from terminal*/
int32_t terminal_read(int32_t fd,void * buf, int32_t nbytes){
    int32_t i;
  
    int32_t result_size;
    int8_t * target_buffer;
    /* Null pointer or number of bytes < 0 */
    if(buf == NULL || nbytes < 0){
        return -1;
    }
    sti();
    while(enter_pressed==0);
    
    /* Get sizes of buffer and decide how many byte to read */
    result_size = written_size;
    target_buffer = (int8_t *)buf;
    if(nbytes< result_size){
        result_size = nbytes;
       
    }
    /* Get typed buffer */
   
    for(i=0;i<result_size;i++){
        target_buffer[i] = terminal_buf[i];
    }
    enter_pressed = 0;
    return result_size;
}
/* int32_t terminal_write();
 *   Inputs: int32_t fd,void * buf, int32_t nbytes
 *   Return Value: number of bytes write terminal
 *    Function: read from terminal*/
int32_t terminal_write(int32_t fd,const void * buf, int32_t nbytes){
    int i;
    int8_t * target_buffer;
    int result_size;
    /* Null pointer or number of bytes < 0 */
    
    if(buf == NULL || nbytes < 0){
        
        return -1;
    }
    /* print to terminal */
    result_size = nbytes;
    
    // if(nbytes < result_size){
    //     result_size = nbytes;
    // }

    target_buffer = (int8_t *)buf;
    for(i=0;i<result_size;i++){
        putc(target_buffer[i]);
        
    }
    return i;
   

}
