#include <stdio.h>
#include <stdlib.h>

#define ERROR 100
FILE *fprt;

int main(int argc, char *argv[])
{
  char file_name[100];   // Might wonna change it

  //scanf("%99s", file_name); // Room for NULL terminator

  if (!scanf("%99s", file_name)){
    printf("ERROR: Invalid input!");
    return ERROR;
  }
  
  fprt = fopen(file_name, "r");

  if (!(fprt != NULL)){
    printf("ERROR: File does not exist!");
    return ERROR;
  }

  char buffer[100];   // Buffer for storing file line contents
  while (fgets(buffer, 100, fprt)){
    printf("%s", buffer);
  }

  fclose(fprt);
  
  return 0;
}
