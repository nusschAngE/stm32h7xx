
#include "public.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <memory.h>



/**********************************/
<<<<<<< HEAD
int pkgStrcmp(const char *str1, const char *str2)
=======
int myStrcmp(const char *str1, const char *str2)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
{
    return strcmp(str1, str2);
}

<<<<<<< HEAD
void *pkgMemset(void *s, int ch, size_t n)
=======
void *myMemset(void *s, int ch, size_t n)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
{
    return memset(s, ch, n);
}

<<<<<<< HEAD
char *pkgStrcpy(char* dest, const char *src)
=======
char *myStrcpy(char* dest, const char *src)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
{
    return strcpy(dest, src);
}

<<<<<<< HEAD
char *pkgStrcat(char *dest, const char *src)
=======
char *myStrcat(char *dest, const char *src)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
{
    return strcat(dest, src);
}
