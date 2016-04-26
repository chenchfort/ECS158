// Threads-based program to find the number of primes between 2 and n;
// Uses Sieve of Eratosthenes, deleting all multiples of 2, all
// multiples of 3, all multiples of 5, etc.

// Unix compilation: gcc -g -o primesthreads PrimesThreads.c -lpthread -lm

// usage: primesthreads n num_threads

#include <math.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_N 10000000
#define MAX_THREADS 25

// Shared variables
int nthreads,          // Number of threads (not counting main())
    n,                 // Range to check for primeness
    prime[MAX_N + 1],  // In the end, prime[i] = 1 if i prime, else 0
    nextbase;          // Next sieve multiplier to be used

// Lock for the shared variable nextbase
pthread_mutex_t nextbaselock = PTHREAD_MUTEX_INITIALIZER;
// ID structs for the threads
pthread_t id[MAX_THREADS];

// "Crosses out" all odd multiples of k
void crossout(int k) {
  int i;
  for (i = 3; i * k <= n; i += 2) {
    prime[i * k] = 0;
  }
}

// Each thread runs this routine
void *worker(int tn)  // tn is the thread number (0, 1,...)
{
  int lim, base, work = 0;  // amounf of work done by this thread
  // No need to check multipliers bigger than sqrt(n)
  lim = sqrt(n);
  do {
    // Get next sieve multiplier, avoiding dupplication across threads
    // Lock the lock
    pthread_mutex_lock(&nextbaselock);
    base = nextbase;
    nextbase += 2;
    // Unlock
    pthread_mutex_unlock(&nextbaselock);
    if (base <= lim) {
      // Don't bother crossing out if base known composite
      if (prime[base]) {
        crossout(base);
        work++;  // Log work done by this thread
      }
    } else {
      return work;
    }
  } while (1);
}

int main(int argc, char **argv) {
  int nprimes,  // Number of primes found
      i, work;
  n = atoi(argv[1]);
  nthreads = atoi(argv[2]);
  // Mark all even numbers nonprime, and the rest "prime until shown otherwise"

  for (i = 3; i <= n; i++) {
    if (i % 2 == 0)
      prime[i] = 0;
    else
      prime[i] = 1;
  }
  nextbase = 3;
  // Get threads started
  for (i = 0; i < nthreads; i++) {
    // This call says create a thread, record its ID in the array id,
    // and get the thread started executing the function worker(),
    // passing the argument i to that function
    pthread_create(&id[i], NULL, worker, i);
  }

  // Wwait for all done
  for (i = 0; i < nthreads; i++) {
    // This call says wait until thread number id[i] finishes
    // execution, and to assign the return value of that thread to our local
    // variable work here
    pthread_join(id[i], &work);
    printf("%d values of base done\n", work);
  }

  // Report results
  nprimes = 1;
  for (i = 3; i <= n; i++) {
    if (prime[i]) {
      nprimes++;
    }
  }
  printf("The number of primes found was %d\n", nprimes);
}
