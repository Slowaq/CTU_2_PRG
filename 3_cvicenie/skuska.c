#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void repeat(int q, const char* x);

int main(int argc, char *argv[]){
    int i;
    int j = scanf("%d", &i);
    
    int next_c = getchar();
    if (next_c != EOF && next_c != '\n'){
        fprintf(stderr, "WARNING: Excess characters detected!\n");
    }

    if (j == 1 && 1 <= i && i <= 10){
        for (int k = i; k > 0; --k){
            repeat(k, "*");
            printf("\n");
        }
    }
}

void repeat(int q, const char* x){
    // Prints q number of x back to back in one line
    for (int p = 0; p < q; p++) {
        printf("%s", x);
    }
}
