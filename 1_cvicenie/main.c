#include <stdio.h>
 
int main()
{
    printf("Hello PRG!\n");

    int i;
    float x;
    scanf("%d %f", &i, &x);
    printf("i, x: %d, %f\n", i,x);

    if (i <= 5){
        for(int k = 0; k < i; k++){
            printf("Ahoj %d\n", k);
        }
    }

    return 0;
}