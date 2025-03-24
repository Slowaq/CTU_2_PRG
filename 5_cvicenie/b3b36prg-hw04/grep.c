#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <regex.h>


#define ERROR 100
#define TU_SOM printf("%s", "TU SOM\n");
FILE *fprt;

int str_equals(const char *a, const char *b);
void insert(char arr[], int *n, int pos, char val);
bool is_in_line(char* line, char* string);
bool is_in_line_repetition(char* line, char* string);
bool is_in_line_colour(char* line, char* string);
int match_regex(const char *pattern, const char *text);
//char check_occourence(char* string);

int main(int argc, char *argv[])
{
  bool found = false;   // Default: No line contains string
  int enable_color = 0; // Default: No color
  int flag_E = 0;       // Default: `-E` not set



  const char *pattern = "colou?r";  // Matches "color" or "colour"
  match_regex(pattern, "color");
  match_regex(pattern, "colour");
  match_regex(pattern, "colouur");  // No match


  for (int i = 1; i < argc; ++i){
    if (str_equals(argv[i], "-E")) flag_E = 1;
    else if (str_equals(argv[i], "--color=always")) enable_color = 1;
  }

  if (argc == 2){
    char line[500000];   // Might wonna change it

    while(1){
      if (fgets(line, sizeof(line), stdin) == NULL) {
        break; // Exit if input error (EOF)
      }

      int len_line = 0;     
      while (line[len_line] != '\0') len_line++;

      if (line[len_line - 1] == '\n') {
        line[len_line - 1] = '\0';
      }

      if (len_line == 1) {
        break;
      }

      if (!enable_color){
        if (is_in_line(line, argv[1])){
          printf("%s\n", line);
          found = true;
        }
      } else if (enable_color){ // Dont need this cuz this will never happen here
        if (is_in_line_colour(line, argv[1])){
          printf("%s\n", line);
          found = true;
        }
      }
    }
  } else if (argc == 3 && enable_color == 0 && flag_E == 0){
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
  } else if (argc == 4 && enable_color == 1){
    fprt = fopen(argv[3], "r");
    if (!(fprt != NULL)){
      printf("ERROR: File does not exist!\n");
      return ERROR;
    }
    
    char buffer[600000] = {'\0'};   // Buffer for storing file line contents, change
    while (fgets(buffer, 100000, fprt)){
      if (is_in_line_colour(buffer, argv[2])){
        printf("%s", buffer);
        found = true;
      }
      for(size_t i = 0; i < sizeof buffer; ++i)
      buffer[i] = '\0';
    }
  
    fclose(fprt);
  } else if (argc == 4 && flag_E == 1){
    fprt = fopen(argv[3], "r");
    if (!(fprt != NULL)){
      printf("ERROR: File does not exist!\n");
      return ERROR;
    }
    
    char buffer[600000] = {'\0'};   // Buffer for storing file line contents, change
    while (fgets(buffer, 100000, fprt)){
      if (!match_regex(argv[2], buffer)){
        printf("%s", buffer);
        found = true;
      }
      for(size_t i = 0; i < sizeof buffer; ++i)
      buffer[i] = '\0';
    }
  
    fclose(fprt);
  } else if (argc == 3 && enable_color == 1){
    
    while(1){
      char line[5000000] = {'\0'};   // Might wonna change it
      if (fgets(line, sizeof(line), stdin) == NULL) {
        break; // Exit if input error (EOF)
      }

      int len_line = 0;     
      while (line[len_line] != '\0') len_line++;

      if (line[len_line - 1] == '\n') {
        line[len_line - 1] = '\0';
      }

      if (len_line == 1) {
        break;
      }

      if (is_in_line_colour(line, argv[2])){
        printf("%s\n", line);
        found = true;
      }
    }
  } else if (argc == 3 && flag_E == 1){
    while(1){
      char line[50000] = {'\0'};   // Might wonna change it
      if (fgets(line, sizeof(line), stdin) == NULL) {
        break; // Exit if input error (EOF)
      }

      int len_line = 0;     
      while (line[len_line] != '\0') len_line++;

      if (line[len_line - 1] == '\n') {
        line[len_line - 1] = '\0';
      }

      if (len_line == 1) {
        break;
      }

      if (!match_regex(argv[2], line)){
        printf("%s\n", line);
        found = true;
      }
    }
  }
  
  
  // } else {
  //   fprt = fopen(argv[2], "r");

  //   if (!(fprt != NULL)){
  //     printf("ERROR: File does not exist!\n");
  //     return ERROR;
  //   }

  //   char buffer[100];   // Buffer for storing file line contents, change
  //   while (fgets(buffer, 100, fprt)){
  //     if (is_in_line_colour(buffer, argv[1])){
  //       printf("%s", buffer);
  //       found = true;
  //     }
  //   }
  
  //   fclose(fprt);
  // }

  
  if (!found) return 1;
  else return 0;
}

int str_equals(const char *a, const char *b){
  while (*a && *b) {
    if (*a != *b) return 0;
    a++;
    b++;
  }
  return (*a == '\0' && *b == '\0');
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
    }
    if (same) return same;
  }
 }
 return same;
}

bool is_in_line_repetition(char* line, char* string){
  char first_letter = string[0];
  bool same = false;
  int len_line = 0;
  int len_string = 0;
  int repeater = 0;
  char token = '\0';
  while (line[len_line] != '\0') len_line++;
  while (string[len_string] != '\0') len_string++;

  // Search for token specifiing how many times can a preceeding element repeat
  for (int i = 0; i < len_string; i++)
  {
    if (!isalpha(string[i])){
      repeater = i - 1;
      token = string[i];
      break;
    }
  }
  printf("%c", token);
  printf("%d",repeater );
  
  for (int i = 0; i < len_line; ++i){
   if (line[i] == first_letter){
     for (int j = 1; j < len_string; ++j){
        printf("%c",line[i + j] );
        printf("%d\n", j);
       if (j == repeater){
         if (!(line[i + j] == repeater || line[i + j] == string[j + 2] || string[j] == '\0')){
           TU_SOM
           same = false;
           break;
          } else same = true;
        }
        else if ((line[i + j] != string[j] || (i + j) >= len_line) && string[j] != '\0'){
          same = false;
          break;
        } else{
          same = true;
        }
     }
     if (same) return same;
   }
  }
  return same;
 }

 int match_regex(const char *pattern, const char *text) {
  regex_t regex;
  int result;

  // Compile the regex
  result = regcomp(&regex, pattern, REG_EXTENDED);
  if (result) {
      printf("Could not compile regex\n");
      return result;
  }

  // Execute the regex
  result = regexec(&regex, text, 0, NULL, 0);
  if (!result) {
      //printf("Match found: %s\n", text);
  } else if (result == REG_NOMATCH) {
      //printf("No match found for: %s\n", text);
  } else {
      //printf("Regex match failed\n");
  }

  // Free the compiled regex
  regfree(&regex);
  return result;
}

bool is_in_line_colour(char* line, char* string){
  char first_letter = string[0];
  bool same = false;
  int len_line = 0;
  int len_string = 0;
  char before[] = "\033[01;31m\033[K";
  char after[] = "\033[m\033[K";
  int len_before = sizeof(before);
  int len_after = sizeof(after);

 
  while (line[len_line] != '\0') len_line++;
  while (string[len_string] != '\0') len_string++;
 
  for (int i = 0; i < len_line; ++i){
    if (line[i] == first_letter){
      if (len_string == 1) same = true;
      else{
        for (int j = 1; j < len_string; ++j){
          if (line[i + j] != string[j] || (i + j) >= len_line){
            same = false;
            break;
          } else{
            same = true;
          }
        }
      }
      if (same){
        for (int k = 0; k < len_before; k++){ // Insert "ESC[01;31m + ESC[K"
          
          //printf("INSERTING: %c at %d\n", before[k],i + k);
          insert(line, &len_line, i + k, before[k]);
        }
        for (int k = 0; k < len_after; k++){ // Insert "ESC[m + ESC[K"
          // TU_SOM
          // printf("INSERTING: %c at %d\n", after[k],i + len_string + len_before + k);
          insert(line, &len_line, i + len_string + len_before + k - 1, after[k]);
        }
        i +=  len_before + len_after - 2; // Skip past the colored part
      } 
    }
  }
  return same;
 }

void insert(char arr[], int *n, int pos, char val){
  if (val != '\0'){
    // Shift elements to the right
    for (int i = *n; i > pos; i--)
      arr[i] = arr[i - 1];

    // Insert val at the specified position
    arr[pos] = val;

    // Increase the current size
    (*n)++;
  }
}


