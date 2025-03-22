#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define ERROR 100
#define TU_SOM printf("%s", "TU SOM\n");
FILE *fprt;

bool is_in_line(char* line, char* string);

int main(int argc, char *argv[])
{
  bool found = false;
  
  if (argc == 2){
    char line[100];   // Might wonna change it

    // do{ // Room for NULL terminator

    //   scanf("%99s", line); // Room for NULL terminator
    //   if (is_in_line(line, argv[1])){
    //     printf("%s", line);
    //     found = true;
    //   }
    // } while (line != '\n');

    while(1){
      if (fgets(line, sizeof(line), stdin) == NULL) {
        break; // Exit if input error (EOF)
      }

      int len_line = 0;     
      while (line[len_line] != '\0') len_line++;

      if (line[len_line - 1] == '\n') {
        line[len_line - 1] = '\0';
      }

      printf("%d", len_line);
      if (len_line == 1) {
        break;
      }

      if (is_in_line(line, argv[1])){
        printf("%s\n", line);
        found = true;
      }
    }
  } else {
    fprt = fopen(argv[2], "r");

    if (!(fprt != NULL)){
      printf("ERROR: File does not exist!\n");
      return ERROR;
    }
    char buffer[100];   // Buffer for storing file line contents, change
    while (fgets(buffer, 100, fprt)){
      if (is_in_line(buffer, argv[1])){
        printf("%s", buffer);
        found = true;
      }
    }
  
    fclose(fprt);
  }

  
  if (!found) return 1;
  else return 0;
}

bool is_in_line(char* line, char* string){
 char first_letter = string[0];
 bool same = false;
 int len_line = 0;
 int len_string = 0;

 while (line[len_line] != '\0') len_line++;
 while (string[len_string] != '\0') len_string++;

 for (int i = 0; i < len_line; ++i){
  if (line[i] == first_letter){
    for (int j = 1; j < len_string; ++j){
      if (line[i + j] != string[j] || (i + j) >= len_line){
        same = false;
        break;
      } else{
        same = true;
      }
    if (same) return same;
    }
  }
 }
 return same;
}
