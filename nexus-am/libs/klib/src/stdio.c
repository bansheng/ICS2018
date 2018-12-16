#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  int i,len;
  char out[200];
  //val = 0;
  va_start(args,fmt);
  vsprintf(out,fmt,args);
  va_end(args);
  len = strlen(out);
  for(i = 0;i < len;i ++)
  {
      _putc(out[i]);	  
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char c;
  char *str = out;
  const char *tmp;
  char num_s[100];
  int i,j,len,num;
  int flag,field_width;

  for(;*fmt; fmt++)
  {
      if(*fmt != '%')
      {
	  *str++ = *fmt;
	  continue;
      }

      flag = 0;
      fmt++;
      while(*fmt == ' ' || *fmt == '0')
      {
	if(*fmt == ' ')  flag |= 8;
	else if(*fmt == '0') flag |= 1;
	fmt++;
      }
      
      field_width = 0;
      if(*fmt >= '0' && *fmt <= '9')
      {
	      while(*fmt >= '0' && *fmt <= '9')
	      {
		      field_width = field_width*10 + *fmt -'0';
		      fmt++;
	      }
      }
      else if(*fmt == '*')
      {
	      fmt++;
	      field_width = va_arg(ap,int);
      }
      //base = 10;

      switch(*fmt)
      {
	  case 's':
	      tmp = va_arg(ap,char *);
	      len = strlen(tmp);
	      for(i = 0;i < len;i ++)
	      {
		   *str++ = *tmp++;
	      }
	      continue;
	  case 'd': break;
      }

      num = va_arg(ap,int);
      j = 0;
      if(num == 0)
      {
	  num_s[j++] = '0';
      }
      else
      {
	  if(num < 0)
	  {
	      *str++ = '-';
	      num = -num;
	  }
	  //j = 0;
	  while(num)
	  {
	      num_s[j++] = num%10 + '0';
	      num /= 10;
	  }
      }
      if(j < field_width)
      {
	      num = field_width - j;
	      c = flag & 1 ? '0' : ' ';
	      while(num--)
	      {
		      *str++ = c;
	      }
      }
      while(j--)
      {
	  *str++ = num_s[j];
      }
  }
  *str = '\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int  val;
  va_start(args,fmt);
  val = vsprintf(out,fmt,args);
  va_end(args);
  return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif

