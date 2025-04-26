#include <stdio.h>
#include <stdlib.h>

enum{
    OK = 0,
    ERROR_MALLOC = 100,
    ERROR_REALLOC = 101,
    DYNAMIC_INIT_SIZE = 10
};

typedef enum {
    read_lines_OK,
    read_lines_Malloc_error,
    read_lines_Readline_error
} read_lines_e;


char* read_line(void);
read_lines_e read_lines(char ***lines, size_t *n);

int main(int argc, char *argv[]){
    int status = OK;

    char** lines;
    int n_lines;
    // read_lines(&lines, );

    char* line = read_line();
    if (line == NULL){
        status = ERROR_REALLOC;
        goto main_daco;
    }
    printf("%s", line);

    main_daco:
    free(line);
    return status;
}

char* read_line(void)
{
    /*
    Reads line from standart input
    Returns pointer to dynamically allocated buffer containing the line
    On failiture returns NULL
    */

   int capacity = DYNAMIC_INIT_SIZE;
   int len = 0;
   char* line = (char*)malloc(capacity * sizeof(char));
   
   int c;
    while (line != NULL){
        if(capacity <= len){
            char* tmp = (char*)realloc(line, 2 * capacity * sizeof(char));
            if (tmp == NULL){
                free(line);
                return NULL;
            } else{
                line = tmp;
                capacity *= 2;
            }
            //printf(stderr, "Realloc %d\n", capacity);
        }

        c = getchar();
        if (c == EOF || c == '\n') {
            line[len] = '\0'; 
            break;
        } else{
            line[len++] = c;
        }
    }

    return line;
}

read_lines_e read_lines(char ***lines, size_t *n)
{
    *n = 0;
    int capacity = DYNAMIC_INIT_SIZE;
    *lines = (char**)malloc(capacity * sizeof(char**));
    if (*lines == NULL) return read_lines_Malloc_error;

    while(!feof(stdin)){

        (*lines)[*n] = read_line();
        if((*lines)[*n-1] == NULL){
            return read_lines_Readline_error;
        }
    }
}
