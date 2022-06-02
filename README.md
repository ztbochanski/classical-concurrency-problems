# Classical Concurrency Problems

>Exploring classical concurrency problems to outline basic principles of synchronization for use in *concurrent programming.*

## Reference

These problems are presented in [The Little Book of Semaphores](https://greenteapress.com/wp/semaphores/)

>A semaphore is a variable, usually an abstract data type, that is used to control access to a common resource. Think multiple processes needing a resource.

## Concepts

**Mutex Lock:** `0` means locked and acquired by a process, `1` means free to grab.

## Usage
1. **Producer and Consumer**
- `-p`: producer/consumer
   - `-n`: number of producers (required if using -p)
   - `-c`: number of consumers (required if using -p)
   
   *example usage with 2 consumers and 2 producers*
   ```sh
   ./conprobs -p 2 2
   ```
1. **Philosopher**
- `-d`: philosopher's problem
   
   *example usage*
   ```sh
   ./conprobs -d
   ```
1. **Potion Brewer**
- `-b`: potion brewers problem
  
  *example usage*
   ```sh
   ./conprobs -b
   ```
