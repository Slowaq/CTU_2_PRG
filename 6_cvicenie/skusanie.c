#include <stdio.h>
#include <stdlib.h>

enum{
    OK = 0,
    ERROR_MALLOC = 100
};

int main(int argc, char *argv[]){
    int size = 10;
    int status = OK;

    
    int* a = (int*)malloc(size * sizeof(int));
    if (a == NULL) return ERROR_MALLOC;

    printf("a: %p\n", (void*)a);

    free(a);

    return OK;
}
