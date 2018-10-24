
#include "public.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <memory.h>



/**********************************/
int myStrcmp(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}

void *myMemset(void *s, int ch, size_t n)
{
    return memset(s, ch, n);
}

char *myStrcpy(char* dest, const char *src)
{
    return strcpy(dest, src);
}

char *myStrcat(char *dest, const char *src)
{
    return strcat(dest, src);
}
