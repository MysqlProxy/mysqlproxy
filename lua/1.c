#include <stdio.h>
#include <stdlib.h>

void get_str(char *str)
{
    *str = '1';
}
int main(){    
    char str[5];
    str[1] = '\0';
    get_str(str);
    printf("%s\n", str);
}

