#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) { // pass
	size_t size = 0;
	while(*(s++)) size++;
	// printf("%s %d\n", s, (int)size);
	return size;
}

char *strcpy(char* dst,const char* src) {
  char *srcdst = dst;
  while((*dst++ = *src++) != '\0');
  return srcdst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  char *srcdst = dst;
  while(*src != '\0' && n--)
     *dst++ = *src++;
  *dst = '\0';
  return srcdst;
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
	// printf("strncmp1 %s %s %d\n", s1, s2, (int)n);
	int a = 0;
	while( (a = (*s1 - *s2)) == 0 && *s1 && *s2 && n) {
		s1++;
		s2++;
		n--;
	}
	// printf("strncmp2 %d\n", a);
	return a;
}

void* memset(void* v,int c,size_t n) {
	printf("11111111111111111111111111\n");
	void* ret = v;
    while(n--)
    {
        *(char*)v = (char)c;
        v = (char*)v + 1; //移动一个字节
    }
    return ret;
	return NULL;
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
	const char *begin1 = (const char *)s1;
	const char *begin2 = (const char *)s2;
	int a = 0;
	while( (a = (*begin1 - *begin2)) == 0 && n--) {
		begin1++;
		begin2++;
	}
	return a;
}

#endif
