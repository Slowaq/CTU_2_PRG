#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define OK 0
#define ERROR 100
#define ERROR_LENGTH 101

enum{
  STARTING_LENGTH = 20,
  LETTERS_IN_ALPHABET = 26,
};

char* read_input(int *lenght);
char* realocate(char str[], int* capacity);
void decipher(const char str1[], const char str2[]);
void shift(const char str1[], char output[], int offset);
char rotate(char ch, int offset);
int compare(const char str1[], const char str2[]);

int main(int argc, char *argv[])
{
  int cipher_length;
  int echo_length;

  char* cipher = read_input(&cipher_length);
  char* echo = read_input(&echo_length);

  // Check for the same length
  if (cipher_length != echo_length){
    fprintf(stderr, "Error: Chybna delka vstupu!\n");
    free(cipher);
    free(echo);
    return ERROR_LENGTH;
  }

  decipher(cipher, echo);

  // Free the memory when done
  free(cipher);
  free(echo);

  return OK;
}

char* read_input(int *lenght){
  int capacity = STARTING_LENGTH;
  char* buffer = (char*)calloc(capacity,  sizeof(char));

  // Check for succesful allocation
  if (!buffer){
    fprintf(stderr, "Memory allocation failed");
    free(buffer);
    exit(ERROR);     // TODO: danglingcd . (or how tf they're called) pointers echo and cipher
  }

  int pos = 0;
  char ch;

  do{
    // Realocate to a new memory if full
    if (pos >= capacity) buffer = realocate(buffer, &capacity);
  
    scanf("%c", &ch);

    if (ch == '\n' || ch == EOF){
      buffer[pos] = '\0';
      break;
    }
    if (isalpha(ch) == 0){
      fprintf(stderr, "Error: Chybny vstup!\n");
      free(buffer);
      exit(ERROR);
    }
    else{
      buffer[pos] = ch;
      pos++;
    }
  } while (ch != '\n');
  
  *lenght = pos;
  buffer[pos] = '\0'; // Null terminate

  return buffer;
}

char* realocate(char str[], int *capacity){
  *capacity *= 2;
  char* temp_buffer = (char*)realloc(str, *capacity * sizeof(char));

  if (temp_buffer == NULL){
    fprintf(stderr, "Memory Reallocation Failed");
    free(str);
    exit (ERROR);
  }

  return temp_buffer;
}

void decipher(const char str1[], const char str2[]){
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

  shift(str1, shifted_cipher, offset);
  
  printf("%s\n", shifted_cipher);
}

void shift(const char str1[], char output[], int offset){
  int len_cipher = strlen(str1);

  for (int i = 0; i < len_cipher; ++i){
    output[i] = rotate(str1[i], offset);
  }
  output[len_cipher] = '\0'; // Null-terminate
}

char rotate(char ch, int offset) {
  if (isalpha(ch)) {
   int base = isupper(ch) ? 'A' : 'a' - LETTERS_IN_ALPHABET;  // 'A' is 0, 'a' is 26
   int index = ch - base;  // Position in the 52-letter sequence
   int new_index = (index + offset) % (LETTERS_IN_ALPHABET * 2);  // Wrap around in the 52-letter range
   
   // Convert back to character
   return (new_index < LETTERS_IN_ALPHABET) ? ('A' + new_index) : ('a' + new_index - LETTERS_IN_ALPHABET);
  }
  return ch;  // Non-alphabetic characters remain unchanged
}

int compare(const char str1[], const char str2[]){
  int len_cipher = strlen(str1);
  int count = 0;

  for (int i = 0; i < len_cipher; ++i){
    if (str1[i] == str2[i] && (str1[i] != '\0' || str2[i] != '\0'))
      count += 1;
  }
  return count;
}
