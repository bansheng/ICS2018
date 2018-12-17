#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>
#define SCREEN_PORT 0x100
static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;

size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
      uint32_t screen;
      screen = inl(SCREEN_PORT);
      //printf("%d\n",screen);
      info->width = screen >> 16;
      info->height = screen << 16 >> 16;
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
	switch (reg) {
		case _DEVREG_VIDEO_FBCTL: {
			_FBCtlReg *ctl = (_FBCtlReg *)buf;
			int i;
			int size = screen_width() * screen_height();
			for (i = 0; i < size; i ++) fb[i] = i;
			if (ctl->sync) {
			// do nothing, hardware syncs.
			}
			return sizeof(_FBCtlReg);
		}
	}
	return 0;
}

void vga_init() {
}

