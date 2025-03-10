#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ERROR 100
#define MAX_LENGTH 100
#define LIMIT 1000000
#define NUM_OF_PRIMES 78498 // Number of primes between 1 and 10^6 according to the prime number theorem


int natural_number(char* num);
int long_division(char* divident, int divisor, char *quotient);
int find_smallest_prime_divisor(char* num, char *quotient);
void prime_decomposition(char *num);
void convert_int_to_array(int num, char* arr, int size);
void sieve_of_Eratosthenes(void);

int primes[NUM_OF_PRIMES] = {0};

int main(int argc, char *argv[])
{
  char num[MAX_LENGTH + 1];  // Allocate max length + 1 for null terminator

  sieve_of_Eratosthenes();

  while (1){
    if (scanf("%100s", num) != 1 || !natural_number(num)){   // CHANGE CONSTANT
      fprintf(stderr, "Error: Chybny vstup!\n");
      return ERROR;
    }

    if (*num == '0'){
      return 0;
    } else {
      prime_decomposition(num);
    }
  }
  return 0;
}

void sieve_of_Eratosthenes(void){
  // Creates an array of all the prime numbers between 1 and LIMIT

  bool nums[LIMIT];

  // Create an array containing only true
  for (int i = 2; i < LIMIT; ++i){
    nums[i] = true;
  }
  
  // Mark all multiples of i as false(not prime)
  for (int i = 2; i < LIMIT; ++i){
    if (nums[i]){
      for (int j = 2; j * i < LIMIT; j += 1){
        nums[j * i] = false;
      }
    }
  }
  
  // Array consisting of prime factorisation
  int helper = 0;
  for (int i = 0; i < LIMIT; ++i){
    if (nums[i]){
      primes[helper] = i;
      ++helper;
    }
  }
}

int natural_number(char* num){
  int len = strlen(num);

  for (int i = 0; i < len; ++i){
    if (!isdigit(num[i])){
      return 0;
    }
  }
  return 1;
}

int find_smallest_prime_divisor(char* num, char *quotient){
  for (int i = 0; i < NUM_OF_PRIMES; ++i){
    if (long_division(num, primes[i], quotient) == 0){
      return primes[i];
    }
  }
  return 0;
}

void prime_decomposition(char* num){
  int decomposition[LIMIT] = {0};
  char quotient[MAX_LENGTH + 1];   // Allocate max length + 1 for null terminator
  // int remainder = long_division(num, 995663, quotient);
  char temp[MAX_LENGTH + 1];
  strcpy(temp, num);
  int divisor = find_smallest_prime_divisor(num, quotient);
  bool first = true;



  while (strcmp(temp, "1") != 0){
    // printf("\nQuotient: %s\n", temp);
    // // printf("Remainder: %d\n", remainder);
    // printf("Prime divisor: %d\n", divisor);
    
    strcpy(temp, quotient);  // Copy quotient into temp
    memset(quotient, 0, MAX_LENGTH + 1);
    if (divisor == 0){
      fprintf(stderr, "Error: Prime factor exceeds 10^6!\n");
      exit(ERROR);
    } 
    decomposition[divisor - 2] += 1;

    divisor = (find_smallest_prime_divisor(temp, quotient));
  }

    // Print output
    printf("Prvociselny rozklad cisla %s je:\n", num);

    for (int i = 0; i < LIMIT; ++i) {
      if (decomposition[i] != 0) {
        if (!first) {
            printf(" x ");  // Print separator before all except the first factor
        }
        if (decomposition[i] != 1) {
            printf("%d^%d", i + 2, decomposition[i]);
        } else {
            printf("%d", i + 2);
        }
        first = false;  
      }
    }
  
    if (first) {  // If no factors were printed, it's a prime number itself
        printf("%s", num);
    }
  
  printf("\n");
}

void convert_int_to_array(int num, char* arr, int size){
  int num_copy = num;
  int num_len = 0;

  // Count number of digits in num
  while (num_copy){
    num_len++;
    num_copy /= 10;
  }

  if (num_len >= size){ // Ensure correct size of the buffer
    fprintf(stderr, "Error: Buffer too small!\n");
    return;
  }

  char arr_reversed[num_len];

  // Separate last digit from num and 
  // add its ASCII value to 48
  int index = 0;
  while (num){
    arr_reversed[index++] = num % 10 + '0';
    num /= 10;
  }

  // Reverse back the array
  for (int i = 0; i < num_len; ++i){
    arr[i] = arr_reversed[num_len - i - 1];
  }

  arr[num_len] = '\0'; // Null-terminate the string

  // for (int i = 0; i < num_len; i++)
  //   printf("%c, ", arr[i]);
}

int long_division(char* divident, int divisor, char *quotient) {
  char divisor_arr[12]; // 12 is enough for any 32-bit int
  convert_int_to_array(divisor, divisor_arr, sizeof(divisor_arr));

  //int divisor_len = strlen(divisor_arr);
  //char quotient[MAX_LENGTH + 1] = {0};  // Allocate max length + 1 for null terminator


  int remainder = 0;
  int q_index = 0;   // Track position of the quotient
  int divident_len = strlen(divident);


  for (int i = 0; i < divident_len; ++i){
    int current_digit = divident[i] - '0';  // Convert char into int
    int num = remainder * 10 + current_digit;

    int quotient_digit = (num / divisor) + '0';
    
    if (q_index > 0 || quotient_digit != '0') {  // Skip leading zeros
      quotient[q_index++] = quotient_digit;
    }
  

    remainder = num % divisor;
  }

  if (q_index == 0){
    quotient[q_index++] = '0';
  }

  quotient[q_index] = '\0'; // Null-terminate the quotient string

  return remainder;
}
