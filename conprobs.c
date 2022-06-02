#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define SHARED 1

// semaphores
sem_t empty, full, mutex_lock;
int data;

//
// producer - create a producer: set wait and lock resources, display data
//
void *producer(void *num)
{
  int *thread = (int *)num;
  printf("\n\nProducer created!\n");
  printf("Producer id-> %ld\n", pthread_self());

  // set thread lock and wait
  printf("..Locked, wait buffer full...\n");
  sem_wait(&full);
  sem_wait(&mutex_lock);
  printf("Thread: %d\n", *thread);

  // ...processing logic

  // free lock and signal
  sem_post(&mutex_lock);
  sem_post(&empty);
  printf("..Unlocked, buffer empty...\n");
}

//
// consumer - create a consumer: set wait and lock resources, display data
//
void *consumer(void *num)
{
  int *thread = (int *)num;
  printf("\n\nConsumer created!\n");
  printf("Consumer id-> %ld\n", pthread_self());

  // set thread lock and wait
  printf("..Locked, wait buffer full...\n");
  sem_wait(&full);
  sem_wait(&mutex_lock);
  printf("Thread: %d\n", *thread);

  // ...processing logic

  // free lock and signal
  sem_post(&mutex_lock);
  sem_post(&empty);
  printf("..Unlocked, buffer empty...\n");
}

void consumer_producer(int prdcr, int cnsmr)
{

  // initialize semaphores:
  // sem_init() initializes the unnamed semaphore at the address
  //    pointed to by sem.  The value argument specifies the initial
  //     value for the semaphore.
  sem_init(&empty, SHARED, 1);
  sem_init(&full, SHARED, 0);
  sem_init(&mutex_lock, SHARED, 1);

  // process producers
  for (int i = 0; i < prdcr; i++)
  {
    // create parent thread
    pthread_t ptid;
    int x = i + 1;
    int *process = &x;
    // system calls to create threads for the producer
    pthread_create(&ptid, NULL, producer, (void *)process);
    // ensure the next thread starts only after ptid is complete
    pthread_join(ptid, NULL);
  }

  // process consumers
  for (int i = 0; i < cnsmr; i++)
  {
    // create child process
    pthread_t ctid;
    int y = i + 1;
    int *process = &y;
    // system calls to create threads for the consumer
    pthread_create(&ctid, NULL, consumer, (void *)process);
    // ensure next starts only after ctid is complete
    pthread_join(ctid, NULL);
  }
  printf("\nProducer problem completed!\n");
}

//
// main -- process command line
//
int main(int argc, char *argv[])
{

  // option args for a valid problem must be entered, warn otherwise
  if (argc == 1)
  {
    fprintf(stderr, "usage: conprobs - enter a valid option -p, -d, -b\n");
    return 1;
  }

  // parse argument array and process arguments
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-p") == 0)
    {
      if (argc <= 2)
      {
        fprintf(stderr, "usage: conprobs -p, enter a valid option -n, -c\n");
        return 1;
      }
      else
      {
        // -p consumer producer problem
        int producer = atoi(argv[++i]);
        int consumer = atoi(argv[++i]);
        fprintf(stderr, "producer count: %d consumer count %d\n", producer, consumer);
        consumer_producer(producer, consumer);
      }
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      // -d philosopher's problem
      // int option = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-b") == 0)
    {
      // -b potion brewers problem
      // int option = atoi(argv[++i]);
    }
    else
    {
      fprintf(stderr, "invalid argument, options: -p, -d, -b\n");
      exit(1);
    }
  }
}
