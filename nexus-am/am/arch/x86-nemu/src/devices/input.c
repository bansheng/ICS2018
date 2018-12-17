#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>
#define I8042_DATA_PORT 0x60 //默认键盘端口

unsigned long long press;
size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      //press = 1;
      //printf("key1=%d\n",(uint32_t)press);
      press = inl(I8042_DATA_PORT);
      //printf("key2=%d\n",(uint32_t)press);
      kbd->keycode = press;
      kbd->keydown = ~(kbd->keydown);
/*      if(press != _KEY_NONE)*/
/*         kbd->keydown = 1;*/
/*      else*/
/*         kbd->keydown = 0;*/
      return sizeof(_KbdReg);
    }
  }
  return 0;
}

