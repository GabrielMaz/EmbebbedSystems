#include <stdio.h>

int main()
{
    int array [50];
    int i;

    for (i = 0; i < 50; i++){
        array[i] = i;
        printf("value of i: %d\n", i);
    }

    return 0;
}