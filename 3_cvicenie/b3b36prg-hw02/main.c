#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR 100

enum{
  NUM_OF_PRIMES = 78498, // Number of primes between 1 and 10^6 according to the prime number theorem
  LIMIT = 1000000,
  COUNT_LIMIT = 100002
};

void prime_decomposition(long long int num);
void sieve_of_Eratosthenes(void);
int find_smallest_prime_divisor(long long int);
int primes[NUM_OF_PRIMES] = {0};

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
    prime_decomposition(num);
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

  for (int i = 0; i < NUM_OF_PRIMES; ++i){
    if (num >= primes[i]){
      if (num % primes[i] == 0){
        return primes[i];
      }
    } else return 0;
  }
  return 0;
}

void prime_decomposition(long long int num) {
  long long int temp = num;
  bool first = true;

  printf("Prvociselny rozklad cisla %lld je:\n", num);

  for (int i = 0; i < NUM_OF_PRIMES && primes[i] * primes[i] <= temp; i++) {
    int prime = primes[i];
    int count = 0;

    while (temp % prime == 0) { // Count how many times the prime divides num
      temp /= prime;
      count++;
    }

    if (count > 0) {
      if (!first) {
        printf(" x ");
      }
      if (count > 1) {
        printf("%d^%d", prime, count);
      } else {
        printf("%d", prime);
      }
      first = false;
    }
  }

  // If temp is still greater than 1, it must be a prime number itself
  if (temp > 1) {
      if (!first) {
          printf(" x ");
      }
      printf("%lld", temp);
  }

  putchar('\n');
}


