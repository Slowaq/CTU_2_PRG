#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define OK 0
#define ERROR 100
#define ERROR_LENGHT 101

enum{
  STARTING_LENGHT = 20
};


int main(int argc, char *argv[])
{
  char *cipher = malloc(STARTING_LENGHT * sizeof(char));
  char *echo = malloc(STARTING_LENGHT * sizeof(char));

  // Check for succesful allocation
  if (!cipher || !echo){
    fprintf(stderr, "Memory allocation failed");
    return ERROR;
  }

  // Populate the array
  for (int i = 0; i < STARTING_LENGHT; ++i){
    cipher[i] = '\0';
    echo[i] = '\0';
  }

  int position = 0;
  int cipher_lenght = 0;
  int lenght = STARTING_LENGHT;
  char ch;

  do{
    // Realocate to a new memory if full
    if (position >= lenght){
      lenght *= 2;
      cipher = realloc(cipher, lenght * sizeof(char));
      echo = realloc(echo,lenght * sizeof(char));

      if (cipher == NULL || echo == NULL){
        fprintf(stderr, "Memory Reallocation Failed");
        return ERROR;
      }
    }

    scanf("%c", &ch);

    if (ch == '\n'){
      cipher[position] = '\0';
      break;
    }
    if (isalpha(ch) == 0){
      fprintf(stderr, "Error: Chybny vstup!\n");
      return ERROR;
    }
    else{
      cipher[position] = ch;
      position++;
    }
  } while (ch != '\n');

  cipher_lenght = position;
  position = 0;
  do{
    scanf("%c", &ch);
  
    if (ch == '\n' || ch == EOF){
      echo[position] = '\0';
      break;
    }
    if (isalpha(ch) == 0){
      fprintf(stderr, "Error: Chybny vstup!\n");
      return ERROR;
    }
    else{
      echo[position] = ch;
      position++;
    }
  } while (ch != '\n');

  // Check for the same lenght
  if (cipher_lenght != position){
    fprintf(stderr, "Error: Chybna delka vstupu!");
    return ERROR_LENGHT;
  }


  printf("JHAKO: %s\n", cipher);
  printf("JHAKO: %s\n", echo);

  // Free the memory when done
  free(cipher);
  free(echo);

  return OK;
}
