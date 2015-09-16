#include "other.h"

void* my_memcpy(void* dest, const void* src, unsigned int count) 
{
	char* dst8 = (char*)dest;
	char* src8 = (char*)src;
        
	if (count & 1) 
	{
		dst8[0] = src8[0];
		dst8 += 1;
		src8 += 1;
	}
        
	count /= 2;
	while (count--) 
	{
		dst8[0] = src8[0];
		dst8[1] = src8[1];
		dst8 += 2;
		src8 += 2;
	}
	
	return dest;
}

void* my_memset(void* s, int c, unsigned int sz) 
{
    char* p = (char*)s;

    /* c should only be a byte's worth of information anyway, but let's mask out
     * everything else just in case.
     */
    char x = c & 0xff;

    while (sz--)
        *p++ = x;
    return s;
}


