#include "other.h"

void* my_memcpy(void* dest, const void* src, unsigned int count) 
{
	char* dst8 = (char*)dest;
	char* src8 = (char*)src;

	while (count--) 
	{
		*dst8++ = *src8++;
	}
	return dest;
}

void* my_memset(void *s, int c, unsigned int n)
{
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}


