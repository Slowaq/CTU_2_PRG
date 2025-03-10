#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR 100

enum{
  NUM_OF_PRIMES = 78498, // Number of primes between 1 and 10^6 according to the prime number theorem
  LIMIT = 1000000
};

void prime_decomposition(long long int num);
void sieve_of_Eratosthenes(void);
int find_smallest_prime_divisor(long long int);
int primes[NUM_OF_PRIMES] = {0};
int binary_search(int arr[], int left, int right, int target);

int main(int argc, char *argv[])
{
  long long int num;

  sieve_of_Eratosthenes();

  while (1){
    if (scanf("%lld", &num) != 1 || num < 0) {
      fprintf(stderr, "Error: Chybny vstup!\n");
      return ERROR;
    }
    if (num == 0) return 0;

    if (num == 1) {
      printf("Prvociselny rozklad cisla 1 je:\n1\n");
      continue;
    }
    if (binary_search(primes,0, NUM_OF_PRIMES - 1, num)){
      printf("Prvociselny rozklad cisla %lld je:\n%lld\n", num,num);
    } else prime_decomposition(num);
  }
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

int find_smallest_prime_divisor(long long int num){
  // Checks for divisibility of num by some prime

  if (binary_search(primes, 0, NUM_OF_PRIMES - 1, num)) return num;

  for (int i = 0; i < NUM_OF_PRIMES; ++i){
    if (num >= primes[i]){
      if (num % primes[i] == 0){
        return primes[i];
      }
    } else return 0;
  }
  return 0;
}

void prime_decomposition(long long int num){
  int decompositoin[LIMIT] = {0};
  long long int temp = num;
  int division = find_smallest_prime_divisor(num);
  bool first = true;

  // Preform prime decomposition
  while (division != 0 && temp >= primes[0]){
    printf("NUm");
    temp = temp / division;
    decompositoin[division - 2] += 1;
    if (temp == division){
      printf("TU");
      decompositoin[division - 2] += 1;
      temp = 0;
    } else{
      division = find_smallest_prime_divisor(temp);
    }
  }

  // Print output
  printf("Prvociselny rozklad cisla %lld je:\n", num);

  for (int i = 0; i < LIMIT; ++i) {
    if (decompositoin[i] != 0) {
      if (!first) {
          printf(" x ");  // Print separator before all except the first factor
      }
      if (decompositoin[i] != 1) {
          printf("%d^%d", i + 2, decompositoin[i]);
      } else {
          printf("%d", i + 2);
      }
      first = false;  
    }
  }

  if (first) {  // If no factors were printed, it's a prime number itself
      printf("%lld", num);
  }

putchar('\n');
}

int binary_search(int arr[], int left, int right, int target) {
  while (left <= right) {
    int mid = left + (right - left) / 2;
    
    if (arr[mid] == target) {
        return 1;  // Found
    } else if (arr[mid] < target) {
        left = mid + 1;
    } else {
        right = mid - 1;
    }
  }
  return 0;  // Not found
}
