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

enum {TYPE_INT, TYPE_UINT, TYPE_HEX, TYPE_STR};

int vsprintf(char *out, const char *fmt, va_list ap) {
	char c;
	char *str = out;
	const char *tmp;
	char num_s[100];
	int i,j,len,num;
	int flag,field_width;
	unsigned int type = TYPE_INT;

	for(;*fmt; fmt++)
	{
		if(*fmt != '%') //一般字符
		{
			*str++ = *fmt;
			continue;
		}

		flag = 0;
		fmt++; //取下一个字符
		while(*fmt == ' ' || *fmt == '0') //前缀
		{
			if(*fmt == ' ')  flag |= 8;
			else if(*fmt == '0') flag |= 1;
			fmt++;
		}

		field_width = 0;// 设置字宽
		if(*fmt >= '0' && *fmt <= '9')
		{
			while(*fmt >= '0' && *fmt <= '9')
			{
				field_width = field_width*10 + *fmt -'0';
				fmt++;
			}
		}
		else if(*fmt == '*') //支持参数设置字宽
		{
			fmt++;
			field_width = va_arg(ap,int);
		}
		//base = 10;

		num = 0;
		switch(*fmt)
		{
			case 's':
				tmp = va_arg(ap, char *);
				len = strlen(tmp);
				for(i = 0;i < len;i ++)
				{
					*str++ = *tmp++;
				}
				continue;
			case 'd': 
				type = TYPE_INT;
				num = va_arg(ap, int);
				break;
			case 'X':  
				num = va_arg(ap, unsigned int);
				*str++ = '0';
				*str++ = 'X';
				type = TYPE_HEX;
				break; 
			case 'u': 
				num = va_arg(ap, unsigned int);
				type = TYPE_UINT;
				break;
		}

		j = 0;
		if(num == 0)
		{
			num_s[j++] = '0';
		}
		else
		{
			if(num < 0 && type == TYPE_INT)
			{
				*str++ = '-';
				num = -num;
			}
			
			if(type != TYPE_HEX)
			{
				while(num)
				{
					num_s[j++] = num%10 + '0';
					num /= 10;
				}
			}
			else //16进制
			{
				while(num)
				{
					int result = num%16;
					if(result < 10)
						num_s[j++] = result + '0';
					else
						num_s[j++] = result + 'A';
					num /= 16;
				}
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

