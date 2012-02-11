#include <stdio.h>
#include <stdlib.h>

typedef struct st_s {int i;} st_t;
int main()
{
    st_t** ls;
    ls = malloc(sizeof(st_t *) * 1);

    printf("1\n");

}
