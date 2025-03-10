#include <stdio.h>

#define STATUS_OK 0
#define ERROR_INPUT 100
#define ERROR_RANGE 101
#define ERROR_EVEN 102
#define ERROR_FENCE 103

enum{
    MIN_SIZE = 3,
    MAX_SIZE = 69
};

void house(int a, int b);
void house_fence(int a, int b, int c);
void roof(int a, int b);
void fence(int c, int horizontal);
void repeat(int q, const char* x);
void body_with_fence(int a, int b, int c, int row, int column);
int input_validation(int *a, int *b, int *c, int *has_fence);

int main(int argc, char *argv[]) {
    int a, b, c, has_fence = 0; // a = width, b = height, c = fence height

    int error = input_validation(&a, &b, &c, &has_fence);

    if (error == STATUS_OK) {
        if (has_fence == 0) {
            house(a, b);
        } else {
            house_fence(a, b, c);
        }
        return STATUS_OK;
    } else {
        return error;
    }
}

int input_validation(int *a, int *b, int *c, int *has_fence) {
    int input_a, input_b, input_c = 0;

    input_a = scanf("%d", a);       // Asking for dimensions
    input_b = scanf("%d", b);

    // Error handling
    if (input_a != 1 || input_b != 1) {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return ERROR_INPUT;
    }
    if (!(MIN_SIZE <= *a && *a <= MAX_SIZE && MIN_SIZE <= *b && *b <= MAX_SIZE)) {
        fprintf(stderr, "Error: Vstup mimo interval!\n");
        return ERROR_RANGE;
    }
    if (*a % 2 != 1) {
        fprintf(stderr, "Error: Sirka neni liche cislo!\n");
        return ERROR_EVEN;
    }

    if (*a == *b) {       // If dimensions of the house are the same, ask for fence height
        input_c = scanf("%d", c);

        // Error handling
        if (input_c != 1) {
            fprintf(stderr, "Error: Chybny vstup!\n");
            return ERROR_INPUT;
        }
        if (*c >= *a || *c <= 0) {
            fprintf(stderr, "Error: Neplatna velikost plotu!\n");
            return ERROR_FENCE;
        }

        *has_fence = 1;   // Used for calling the right function
    }
    return STATUS_OK;
}

void roof(int a, int b) {
    // Prints roof by centering 'X' and expanding outward
    for (int k = (a - 1) / 2; k > 0; --k) {
        if (k != (a - 1) / 2) {
            printf("%*sX%*sX\n", k, "", a - (2 * k) - 2, "");
        } else {
            printf("%*sX\n", k, "");
        }
    }

    repeat(a, "X");
    printf("\n");
}

void repeat(int q, const char* x){
    // Prints q number of x back to back in one line
    for (int p = 0; p < q; ++p) {
        printf("%s", x);
    }
}

void fence(int c, int horizontal){
    // Prints right border with fence,
    // if horizontal == 1: "-|"
    // else: " |"
    printf("X");
    if (!horizontal){
        if (c % 2 == 0){
            repeat(c / 2, " |");
        } else{
            printf("|");
            repeat((c - 1) / 2, " |");
        }
    } else{
        if (c % 2 == 0){
            repeat(c / 2, "-|");
        } else{
            printf("|");
            repeat((c - 1) / 2, "-|");
        }
    }
    printf("\n");
}

void body_with_fence(int a, int b, int c, int row, int column){
    if (column == 1) printf("X"); // Left border

    if (column == b - 1) { // Right border
        if (row == a - c) fence(c, 1);   // Horizontal part of the fence
        else if (row >= a - c) fence(c, 0);   // Vertical part of the fence
        else printf("X\n");
    } 
    else { // Inside of the border
        if ((row + column) % 2 == 0) printf("o");
        else printf("*");
    }
}

void house(int a, int b) {
    // Prints house without fill nor fence
    roof(a, b);

    // Printing walls
    for (int l = 1; l <= (b - 2); ++l) {
        printf("X%*sX\n", a - 2, "");
    }

    repeat(a, "X");
    printf("\n");
}

void house_fence(int a, int b, int c) {
    // Prints house with fill and fence
    roof(a, b);

    // Printing walls
    for (int row = 1; row < a - 1; ++row) {
        for (int column = 1; column <= b - 1; ++column) {
            body_with_fence(a, b, c, row, column);
        }
    }

    // Prints bottom border of the house with fence
    repeat(a, "X");
    if (c % 2 == 0){
        repeat(c / 2, "-|");
    } else{
        printf("|");
        repeat((c - 1) / 2, "-|");
    }
    printf("\n");
}
