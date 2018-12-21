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
	return len;
}

/*enum {TYPE_INT, TYPE_UINT, TYPE_HEX, TYPE_STR};*/

int vsprintf(char *out, const char *fmt, va_list ap) {
	char c;
	char *str = out;
	const char *tmp;
	char num_s[100];
	int i,j,len;
	int flag,field_width;
	int num = 0;
	unsigned int num2 = 0;

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

		
		j = 0;
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
			case 'u': 
				num2 = va_arg(ap, unsigned int);
				if(num2 == 0)
				{
					num_s[j++] = '0';
				}
				else
				{
	
					while(num2)
					{
						num_s[j++] = num2%10 + '0';
						num2 /= 10;
					}
				}
				break;
			case 'd': 
				num = va_arg(ap, int);
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
	
					while(num)
					{
						num_s[j++] = num%10 + '0';
						num /= 10;
					}
				}
				break;
			case 'X':  
				num2 = va_arg(ap, unsigned int);
				*str++ = '0';
				*str++ = 'X';
				if(num2 == 0)
				{
					num_s[j++] = '0';
				}
				else
				{
					while(num2)
					{
						int result = num2%16;
						if(result < 10)
							num_s[j++] = result + '0';
						else
							num_s[j++] = result + 'A';
						num2 /= 16;
					}
				}
				break; 
		}

		int size = 0;
		if(j < field_width)
		{
			size = field_width - j;
			c = flag & 1 ? '0' : ' ';
			while(size--)
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
	return strlen(str);
}

int sprintf(char *out, const char *fmt, ...) {
	va_list args;
	int  val;
	va_start(args,fmt);
	val = vsprintf(out,fmt,args);
	va_end(args);
	return val;
}

char mid_buf[65536+1];
int snprintf(char *out, size_t n, const char *fmt, ...) {
	va_list args;
	int  val;
	va_start(args,fmt);
	val = vsprintf(mid_buf,fmt,args);
	va_end(args);
	if(val > n) //越界
	{
		mid_buf[n] = '\0';
	}
	return strlen(strcpy(out, mid_buf));
}

#endif

