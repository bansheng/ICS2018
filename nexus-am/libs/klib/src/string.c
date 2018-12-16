#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) { // pass
	size_t size = 0;
	while(*(s++)) size++;
	// printf("%s %d\n", s, (int)size);
	return size;
}

char *strcpy(char* dst,const char* src) { //pass
	// 注意前后覆盖的问题
	return strncpy(dst, src, strlen(src));
}

char* strncpy(char* dst, const char* src, size_t n) { //pass
	size_t size_src = strlen(src);
	if( n > size_src) return strncpy(dst, src, size_src);
	else // n <= size_src
	{
		// printf("strncpy1 %s %s %d\n", dst, src, (int)n);
		// 注意前后覆盖的问题
		char ch[n+1];
		char *head = ch;
		size_t i = 0;
		while( i < n ) {
			*(head+i) = *(src+i);
			i++;
		} 
		ch[n] = '\0';
		head = ch;
		char *result = dst;
		while((*(dst++) = *(head++))) ;
		// printf("strncpy2 %s\n", dst);
		return result;
	}
}

char* strcat(char* dst, const char* src) { //pass
	// printf("strcat1 %s %s\n", dst, src);
	char *result = dst;
	while(*dst) dst++;
	strcpy(dst, src);
	// printf("strcat2 %s\n", dst);
	return result;
}

int strcmp(const char* s1, const char* s2) { //pass
	// printf("strcmp1 %s %s\n", s1, s2);
	int a = 0;
	while( (a = (*s1 - *s2)) == 0 && *s1 && *s2) {
		s1++;
		s2++;
	}
	// printf("strcmp2 %d\n", a);
	return a;
}

int strncmp(const char* s1, const char* s2, size_t n) { //pass
	assert((s1 != NULL) && (s2 != NULL));
	int a = 0;
	while( (a = (*s1 - *s2)) == 0 && *s1 && *s2 && n--) {
		s1++;
		s2++;
	}
	return a;
}

void* memset(void* v,int c,size_t n) {
	void* ret = v;
    while(n--)
    {
        *(char*)v = (char)c;
        v = (char*)v + 1; //移动一个字节
    }
    return ret;
}

void* memcpy(void* out, const void* in, size_t n) {
	void *ret = out; //可能会出现覆盖的问题
	char *begin1 = (char *)out;
	const char *begin2 = (const char *)in;
	while(n--)
	{
		*(begin1+n-1) = *(begin2+n-1);
	}
	return ret;
}

int memcmp(const void* s1, const void* s2, size_t n){
  /*assert((s1 != NULL) && (s2 != NULL));
  char *tmp1 = (char *)s1;
  char *tmp2 = (char *)s2;
  while(n--){
     while(*tmp1 == *tmp2){
        if(*tmp1 == '\0')
            return 0;
        tmp1++;
        tmp2++;
     }
  }
  if(*tmp1 > *tmp2)
       return 1;
  if(*tmp1 < *tmp2)
       return -1;*/
  return 0;
}

#endif

