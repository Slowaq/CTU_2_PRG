#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define OK 0
#define ERROR 100
#define ERROR_LENGHT 101

enum{
  STARTING_LENGHT = 20,
  LETTERS_IN_ALPHABET = 26,
  JUMP = 6,  // Space between Z and a in ASCII Table
  RIGHT_BRACKET = 91 // ASCII position of [
};

void decipher(const char *str1, const char *str2);
void shift(const char *str1, char* output, int offset);
char rotate(char ch, int offset);
int compare(const char *str1, const char *str2);

int main(int argc, char *argv[])
{
  char *cipher = malloc(STARTING_LENGHT * sizeof(char));
  char *echo = malloc(STARTING_LENGHT * sizeof(char));

  // Check for succesful allocation
  if (!cipher || !echo){
    fprintf(stderr, "Memory allocation failed");
    free(cipher);
    free(echo);
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
        free(cipher);
        free(echo);
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
      free(cipher);
      free(echo);
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
      free(cipher);
      free(echo);
      return ERROR;
    }
    else{
      echo[position] = ch;
      position++;
    }
  } while (ch != '\n');

  // Check for the same lenght
  if (cipher_lenght != position){
    fprintf(stderr, "Error: Chybna delka vstupu!\n");
    free(cipher);
    free(echo);
    return ERROR_LENGHT;
  }

  decipher(cipher, echo);

  // Free the memory when done
  free(cipher);
  free(echo);

  return OK;
}


void decipher(const char *str1, const char *str2){
  int offset = 0;
  int best = 0;
  char shifted_cipher[strlen(str1) + 1]; // Magic number


  for (int i = 0; i < LETTERS_IN_ALPHABET * 2; ++i){  // Is 2 considered a magic number??
    shift(str1, shifted_cipher, i);
    int same = compare(str2, shifted_cipher);
    if (same > best){
      best = same;
      offset = i;
    }
  }

  //printf("%d,%d\n", best, offset);
  shift(str1, shifted_cipher, offset);
  
  printf("%s\n", shifted_cipher);
}

void shift(const char *str1, char* output, int offset){
  int len_cipher = strlen(str1);
  //char temp[len_cipher + 1];

  for (int i = 0; i < len_cipher; ++i){
    output[i] = rotate(str1[i], offset);
  }
  output[len_cipher] = '\0'; // Null-terminate
}

char rotate(char ch, int offset) {
  if (isalpha(ch)) {
      int base = isupper(ch) ? 'A' : 'a' - 26;  // 'A' is 0, 'a' is 26
      int index = ch - base;  // Position in the 52-letter sequence
      int new_index = (index + offset) % 52;  // Wrap around in the 52-letter range
      
      // Convert back to character
      return (new_index < 26) ? ('A' + new_index) : ('a' + new_index - 26);
  }
  return ch;  // Non-alphabetic characters remain unchanged
}



int compare(const char *str1, const char *str2){
  int len_cipher = strlen(str1);
  int count = 0;

  for (int i = 0; i < len_cipher; ++i){
    // printf("%c,%c ", str1[i], str2[i]);
    if (str1[i] == str2[i] && (str1[i] != '\0' || str2[i] != '\0'))
      count += 1;
  }
  // printf("%d\n", count);
  return count;
}
