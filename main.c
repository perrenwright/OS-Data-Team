#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
 
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum - 1) % 5
#define RIGHT (phnum + 1) % 5
 
int state[5];
int phil[5] = { 0, 1, 2, 3, 4 };
 
sem_t mutex;
sem_t S[5];
 
void check(int phnum)
{
   //only one philosopher passed in at a time, making it mutually exclusive
    if (state[phnum] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        // state that eating
        state[phnum] = EATING;
     //deadlock free
     //mutually exclusive in that no 2 neighbors can eat simultaneously
 
        sleep(2);
        printf(" \n");
        printf("*********************************\n");
        printf("Philosopher %d picks up chopsticks\n", phnum + 1);
        printf("Philosopher %d is Eating\n", phnum + 1);
        printf("Philosophers %d & %d cannot eat \n", LEFT+1, RIGHT+1);
        printf("*********************************\n");
        printf(" \n");
 
        // sem_post(&S[phnum]) has no effect
        // during takefork
        // used to wake up hungry philosophers
        // during putdown_chop
        sem_post(&S[phnum]);
    }
}
 
// take up chopsticks
void pickup_chop(int phnum)
{
 
    sem_wait(&mutex);
 
    // state that hungry
    state[phnum] = HUNGRY;
 
    printf("Philosopher %d is Hungry\n", phnum + 1);
 
    // eat if neighbours are not eating
    check(phnum);
 
    sem_post(&mutex);
 
    // if unable to eat wait to be signalled
    sem_wait(&S[phnum]);
 
    sleep(1);
}
 
// put down chopsticks
void putdown_chop(int phnum)
{
 
    sem_wait(&mutex);
 
    // state that thinking
    state[phnum] = THINKING;
 
    printf("Philosopher %d is putting their chopsticks down\n", phnum + 1);
    printf("Philosopher %d is thinking\n", phnum + 1);
 
    check(LEFT);
    check(RIGHT);
 
    sem_post(&mutex);
}
 
void* philospher(void* num)
{
 
    while (1) {
 
        int* i = num;
 
        sleep(1);
 
        pickup_chop(*i);
 
        sleep(0);
 
        putdown_chop(*i);
    }
}
 
int main()
{
 
    int i;
    pthread_t tid[5];
 
    // initialize the semaphores
    sem_init(&mutex, 0, 1);
 
    for (i = 0; i < 5; i++)
 
        sem_init(&S[i], 0, 0);
 
    for (i = 0; i < 5; i++) {
 
        // create philosopher processes
        pthread_create(&tid[i], NULL,
                       philospher, &phil[i]);
 
        printf("Philosopher %d is thinking\n", i + 1);
    }
 
    for (i = 0; i < 5; i++)
 
        pthread_join(tid[i], NULL);
}
