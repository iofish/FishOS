
#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include "types.h"
#define POSIX_YEAR	1970

typedef struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
}datetime_t;

extern uint32_t globalFreq;

// 初始化始终中断
void init_timer(uint32_t frequency);
// sleep 秒
void sleep(uint32_t seconds);
// sleep 微秒
void mSleep(uint32_t milliseconds);
// 系统当前运行时间
int32_t getSystemUpTime();
// 获取RTC时间
datetime_t getDatetime();

unsigned char readCMOS(unsigned char addr);
#endif 	// INCLUDE_TIMER_H_
