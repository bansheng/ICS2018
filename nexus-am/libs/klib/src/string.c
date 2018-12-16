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

char* strcat(char* dst, const char* src) {
  char *temp = dst;
  while(*dst != '\0')
     dst++;
  while((*dst++ = *src++) != '\0');
  return temp;
}

int strcmp(const char* s1, const char* s2) {
  assert((s1 != NULL) && (s2 != NULL));
  while( *s1 && *s2 && *s1==*s2){
     ++s1;
     ++s2;
  }
  return (*s1 - *s2);
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert((s1 != NULL) && (s2 != NULL));
  while( *s1 && *s2 && *s1==*s2 && n--){
     ++s1;
     ++s2;
  }
  return (*s1 - *s2);
}

void* memset(void* v,int c,size_t n) {
  char * pvTo=(char *)v;
  assert(v != NULL);
  while(n-- >0)
    *pvTo++ = (char)c;
  *pvTo = '\0';
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char * pbIn = (char *)in;
  char * pbOut = (char *)out;
  assert(pbIn != NULL && pbOut != NULL);//不能存在空指针
  assert(pbIn >= pbOut+n || pbOut >= pbIn+n);//防止内存重叠
  while(n-- >0)
     *pbOut++ = *pbIn++;
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert((s1 != NULL) && (s2 != NULL));
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
       return -1;
  return 0;
}

#endif

