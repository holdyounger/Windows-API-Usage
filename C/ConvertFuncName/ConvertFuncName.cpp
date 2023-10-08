/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby,
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "malloc.h"
// GetHost -> get_host

char* change(const char* str)
{
    if (strlen(str) == 0)
        return nullptr;

    int i = strlen(str);

    char* ret = (char*)malloc(strlen(str) * 2);
    // char ret[256];

    if(ret != nullptr)
        memset(ret, NULL, sizeof(ret));

	int j = 0;
	if (str[0] >= 'A' && str[0] <= 'Z')
	{
        ret[j++] = str[0] - 'A' + 'a';
        for (int i = 1; i < strlen(str); i++)
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
            {
                ret[j++] = '_';
                ret[j++] = str[i] - 'A' + 'a';
            }
            else
                ret[j++] = str[i];
        }
    }
    else
    {
        if (str[0] >= 'a' && str[0] <= 'z')
        {
            ret[j++] = str[0] - 'a' + 'A';
            for (int i = 1; i < strlen(str); i++)
            {
                if (str[i] == '_')
                {
                    ret[j++] = str[++i] - 'a' + 'A';
                }
                else
                    ret[j++] = str[i];
            }
        }
    }

    if(j < sizeof(ret))
        ret[j] = '\0';

    return ret;
}

int main()
{

    // char str[] = "HelloWorld";
    char str[] = "hello_world";

    char* ret = change(str);

    printf("%s", ret);

    free(ret);

    return 0;
}
