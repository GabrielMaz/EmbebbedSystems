#include <stdio.h>

int main()
{
    int array [50];
    int i;

    i = 0;

    while (i < 50){
      array[i] = i;
      printf("value of i: %d\n", i);
      i++;
    }

    return 0;
}