#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#define BUFFER 1

// ---------------------------------------------------------------------------------------------------------------------
// POTION BREWERS PROBLEM
// ---------------------------------------------------------------------------------------------------------------------

// counting semaphore for each brewer
// each brewer has a unique infinite supply of bezoars, unicorn horns, or mistletoe berries.
sem_t brewers[3];
// binary semaphore for the agent who give out ingredients
sem_t agent;
// define custom type for brewers and their type
typedef void *(*brewers_t)(void *);
// legal potion brew limit per epoch for brewers
int BREW_LIMIT = 1 // potions/epoch

                 //
                 // brewer with an infinite supply of bezoars
                 //
                 void *
                 potion_brewer_with_bezoars(void *arg)
{
  while (BREW_LIMIT != 0)
  {
    sleep(1);
    printf("\nBezoars brewer waiting to brew...)\n");
    // brewer 0 the bezoars brewer is locked
    sem_wait(&brewers[0]);
    printf("Bezoars brewer now has bezoars, unicorn horns, and mistletoe berries... brewing...\n");
    // signal the agent that the resource is free and in need of it again
    sem_post(&agent);
    BREW_LIMIT--;
  }
  return NULL;
}

//
// brewer with an infinite supply of unicorn horns
//
void *
potion_brewer_with_unicorn_horns(void *arg)
{
  while (BREW_LIMIT != 0)
  {
    sleep(1);
    printf("\nUnicorn brewer waiting to brew...)\n");
    // brewer 0 the unicorn_horns brewer is locked
    sem_wait(&brewers[1]);
    printf("Unicorn brewer now has unicorn horns, bezoars, and mistletoe berries... brewing...\n");
    // signal the agent that the resource is free and in need of it again
    sem_post(&agent);
    BREW_LIMIT--;
  }
  return NULL;
}

//
// brewer with an infinite supply of mistletoe berries
//
void *
potion_brewer_with_mistletoe_berries(void *arg)
{
  while (BREW_LIMIT != 0)
  {
    sleep(1);
    printf("\nMistletoe brewer waiting to brew...)\n");
    // brewer 0 the unicorn_horns brewer is locked
    sem_wait(&brewers[2]);
    printf("Mistletoe brewer now has mistletoe berries, bezoars, and unicorn horns... brewing...\n");
    // signal the agent that the resource is free and in need of it again
    sem_post(&agent);
    BREW_LIMIT--;
  }
  return NULL;
}

//
// agent hands hands out ingredient deliveries to the three brewers
//
void *agent_and_brewers(void)
{
  // initialize binary semaphore for agent
  sem_init(&agent, 0, 1);

  // define threads for each of the brewers
  pthread_t brewer_threads[3];
  brewers_t brewer_types[3] = {
      potion_brewer_with_bezoars,
      potion_brewer_with_unicorn_horns,
      potion_brewer_with_mistletoe_berries,
  };

  // create the brewer threads and init the brewer semaphores
  for (int i = 0; i < 3; ++i)
  {
    // init the brewer semaphore
    sem_init(&brewers[i], 0, 0);

    // create threads, if there it's blocked return 0: linux manual:
    // EAGAIN Resource temporarily unavailable (may be the same value as
    //          EWOULDBLOCK) (POSIX.1-2001).
    if (pthread_create(&brewer_threads[i], NULL, brewer_types[i], NULL) == EAGAIN)
    {
      perror("Could not create thread\n");
      return 0;
    }
  }

  // count for duration of brew cycles
  int ingredient_epochs = 1;
  // agent gives out ingredients
  while (ingredient_epochs != 0)
  {
    // lock agent when they are delivering
    sem_wait(&agent);

    // deliver ingredients to a brewer
    sem_post(&brewers[rand() % 2]);
    ingredient_epochs--;
  }

  // join brewers threads
  for (int i = 0; i < 3; ++i)
  {
    pthread_join(brewer_threads[i], NULL);
  }
}

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
      if (argc > 2)
      {
        fprintf(stderr, "invalid options: use only conprobs -b\n");
        return 1;
      }
      else
      {
        // -b potion brewers problem
        agent_and_brewers();
      }
    }
    else
    {
      fprintf(stderr, "invalid argument, options: -p, -d, -b\n");
      exit(1);
    }
  }
}
