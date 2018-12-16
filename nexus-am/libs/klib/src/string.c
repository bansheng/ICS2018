#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t size = 0;
	while(*(s++)) size++;
	return size;
}

char *strcpy(char* dst,const char* src) {
	// 注意前后覆盖的问题
	char ch[strlen(src)];
	char *head = ch;
	while((*(head++) = *(src++))) ;
	head = ch;
	char *result = dst;
	while((*(dst++) = *(head))) ;
	return result;
}

char* strncpy(char* dst, const char* src, size_t n) {
	return NULL;
}

char* strcat(char* dst, const char* src) {
	return NULL;
}

int strcmp(const char* s1, const char* s2) {
	return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	return 0;
}

void* memset(void* v,int c,size_t n) {
	return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
	return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
	return 0;
}

#endif
