#include <stdio.h>

int main(void){
    int j, valid_input;
    char extra;

    do {
        printf("Enter a number: ");
        valid_input = scanf("%d%c", &j, &extra);

        if (valid_input != 2 || extra != '\n' || j >= 5 || j < 0){
            printf("Invalid number, try again!\n\n");
            if (valid_input != 2){
                while (getchar() != '\n');   // Clear buffer
            }
        }
    } while (valid_input != 2 || extra != '\n' || j >= 5 || j < 0);

    printf("\nPrinting a sequence!\n");
    for(int i = j; i <= 5; i++){
        printf("%d\n", i);
    }
    return 0;
}
