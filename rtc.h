#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define RTC_PORT 0x70
#define CMOS_PORT 0x71
#define REGISTER_A 0x8A
#define REGISTER_B 0x8B
#define REGISTER_C 0x8C
#define REGISTER_D 0x8D
#define RTC_IRQ 8 
#define mask_40 0x40
#define mask_F0 0xF0
#define rate 0x0F
#define NUM_OF_BYTES 4

#define Hz_2 2
#define Hz_4 4
#define Hz_8 8
#define Hz_16 16
#define Hz_32 32
#define Hz_64 64
#define Hz_128 128
#define Hz_256 256
#define Hz_512 512
#define Hz_1024 1024

#define F_2 0x0F
#define F_4 0x0E
#define F_8 0x0D
#define F_16 0x0C
#define F_32 0x0B
#define F_64 0x0A
#define F_128 0x09
#define F_256 0x08
#define F_512 0x07
#define F_1024 0x06
//initialize RTC
extern void change_flag();

extern void init_rtc(void);

extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t rtc_open(const uint8_t * filename);

extern int32_t rtc_close(int32_t fd);

extern void rtc_interrupt_handler(void);

#endif

