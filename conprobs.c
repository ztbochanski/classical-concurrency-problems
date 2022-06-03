#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#define BUFFER 1

// ---------------------------------------------------------------------------------------------------------------------
// DINING PHILOSOPHERS PROBLEM
// ---------------------------------------------------------------------------------------------------------------------

// semaphores for dining philosophers
// binary semaphore
sem_t dining_room;
// counting semaphore
sem_t utensils[4];

//
// current philosopher mutates semaphores to busy and allows philosopher to eat
//
void *current_philosopher(void *num)
{
  int philosopher = *(int *)num;
  sem_wait(&dining_room);
  printf("Philosopher %d has walked in the dining room\n", philosopher + 1);

  // wait for philosopher to use for resource
  sem_wait(&utensils[philosopher]);
  sem_wait(&utensils[(philosopher + 1) % 4]);

  // philosopher uses utensils to eat and then finishes
  printf("Philosopher %d is eating\n", philosopher + 1);
  sleep(1); // wait for effect
  printf("Philosopher %d has finished eating\n", philosopher + 1);

  // allow for others to eat
  sem_post(&utensils[(philosopher + 1) % 4]);
  sem_post(&utensils[philosopher]);
  sem_post(&dining_room);
}

//
// dining philosophers initializes philosopher threads, creates and joins threads
//
void dining_philosophers(void)
{

  // position at table and thread for that philosopher
  int position[4];
  pthread_t thread_id[4];
  // initialize the semaphore
  sem_init(&dining_room, 0, 3);

  // iterate through utensilss (positions at the table) and init the counting semaphores
  for (int i = 0; i < 4; i++)
  {
    sem_init(&utensils[i], 0, 1);
  }

  // create threads for each of the positions
  for (int i = 0; i < 4; i++)
  {
    // assign the the available position to a position
    position[i] = i;
    // create a thread for this seating position
    pthread_create(&thread_id[i], NULL, current_philosopher, (void *)&position[i]);
  }

  // wait for threads to execute then join back to main process
  for (int i = 0; i < 4; i++)
  {
    pthread_join(thread_id[i], NULL);
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// PRODUCER/CONSUMER PROBLEM
// ---------------------------------------------------------------------------------------------------------------------

// semaphores for producer consumer
sem_t empty, full, mutex_lock;

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

//
// consumer producer - create specified number of consumer and producer threads
//
void consumer_producer(int prdcr, int cnsmr)
{

  // initialize semaphores:
  // sem_init() initializes the unnamed semaphore at the address
  //    pointed to by sem.  The value argument specifies the initial
  //     value for the semaphore.
  sem_init(&empty, BUFFER, 1);
  sem_init(&full, BUFFER, 0);
  sem_init(&mutex_lock, BUFFER, 1);

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

// ---------------------------------------------------------------------------------------------------------------------
// SHELL INTERACTION
// ---------------------------------------------------------------------------------------------------------------------

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
      if (argc > 2)
      {
        fprintf(stderr, "invalid options: use only conprobs -d\n");
        return 1;
      }
      else
      {
        // -d philosopher's problem
        dining_philosophers();
      }
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
