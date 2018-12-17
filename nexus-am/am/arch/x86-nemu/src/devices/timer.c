#include <am.h>
#include <x86.h>
#include <amdev.h>
#define RTC_PORT 0X48 //定义的启动时间端口
unsigned long long now_time;
size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
      // inl 针对x86-nemu实现的in函数
      now_time = inl(RTC_PORT);
      uptime->hi = now_time >> 32;
      uptime->lo = now_time & 0xffffffff;
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
}
