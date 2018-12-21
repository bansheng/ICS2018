#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
	uintptr_t i = 0;
	for(; len > 0; len--) {
		_putc(((char*)buf)[i]);
		i++;;
	}
	return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
	[_KEY_NONE] = "NONE",
	_KEYS(NAME) //amdev.h里面的宏定义
};

size_t events_read(void *buf, size_t offset, size_t len) {
	int key_code;

	if ((key_code = read_key()) == _KEY_NONE) 
	{
		snprintf(buf, len, "t %u\n", uptime());
	} 
	else if (key_code & 0x8000) 
	{
		key_code ^= 0x8000;
		snprintf(buf, len, "kd %s\n", keyname[key_code]);
/*		extern void switch_game();*/
/*		if (key_code == _KEY_F12)*/
/*			switch_game();*/
	} 
	else 
	{
		snprintf(buf, len, "ku %s\n", keyname[key_code]);
	}
	return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	strncpy(buf, dispinfo + offset, len);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	int row, col;
	offset /= 4;
	col = offset % screen_width();
	row = offset / screen_width();

	draw_rect((uint32_t *)buf, col, row, len/4, 1);
	return len;
}

void init_device() {
	Log("Initializing devices...");
	_ioe_init();

	// TODO: print the string to array `dispinfo` with the format
	// described in the Navy-apps convention
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
