#ifndef _INT_HANDLER_H
#define _INT_HANDLER_H

#include "types.h"

/* RTC_Interruput */
extern void RTC_int();
/* keyboard_Interruput */
extern void keyboard_int();

/* open terminal */
extern int32_t terminal_open(const uint8_t* filename);
/* read user input */
extern int32_t terminal_read(int32_t fd,void * buf, int32_t nbytes);
/* write to user input */
extern int32_t terminal_write(int32_t fd,const void * buf, int32_t nbytes);

/* close terminal */
extern int32_t terminal_close(int32_t fd);


#endif
