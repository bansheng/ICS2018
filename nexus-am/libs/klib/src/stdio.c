#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsprintf(out,fmt, args);// This still uses standaes formating
	va_end(args);
	return 0;// Before return you can redefine it back if you want...
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	
  	return 0;
}

#endif
