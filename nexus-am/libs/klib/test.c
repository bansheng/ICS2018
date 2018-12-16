#include <stdio.h>

size_t strlen(const char *s) { // pass
	size_t size = 0;
	while(*(s++)) size++;
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
		return result;
	}
}

char* strcat(char* dst, const char* src) { //pass
	char *result = dst;
	while(*dst) dst++;
	strcpy(dst, src);
	return result;
}

int strcmp(const char* s1, const char* s2) { //pass
	int a = 0;
	while( (a = (*s1 - *s2)) == 0 && *s1 && *s2) {
		s1++;
		s2++;
	}
	return a;
}

int strncmp(const char* s1, const char* s2, size_t n) { //pass
	int a = 0;
	while( (a = (*s1 - *s2)) == 0 && *s1 && *s2 && n) {
		s1++;
		s2++;
		n--;
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
	const char *begin1 = (const char *)out;
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

int main(){
	char ch1[30] = "hello, world";
	char ch2[20] = "hello, dyd";
	// printf("%u\n", strlen(ch2));
	printf("%s\n", ch1);
	int a = strncmp(ch1, ch2, 10);
	printf("%d\n", a);
/*	printf("%s\n", ch1);*/
	return 0;
}


