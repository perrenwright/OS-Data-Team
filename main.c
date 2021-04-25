#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#define LEFT (i-1) % 5
#define RIGHT (i+1) % 5
#define HUNGRY 0
#define EATING 1
#define THINKING 2

//binary semaphore
SDL_sem *s[5]; //one semaphore per philosopher
bool quit=false;
SDL_mutex *mutex;
int state[5]; //array keeping track of everyone’s state

void think(int i){
	SDL_Delay ( rand() % 3000);
	// thinking for random time between 0 and 3 seconds
}

void eat (int i){
	SDL_Delay( rand() % 2000);
	// thinking for random time between 0 and 2 seconds

}

void take_chops (int i){
	SDL_Delay (rand() % 500);
}

void check (int i){
	int left = LEFT;
	if(left <0) left+=5;
	//Below checks if left is not eating and right is not eating
	if (state[i] == HUNGRY && state[left] != EATING && state[RIGHT] != EATING){
		state[i]=EATING;
		//if one state = eating then the bottom 2 statements won't be executed
		//if not eating, then signal
		//SemPost= signal s[i]
		SDL_SemPost (s[i]);
		//GOES BACK TO PICKUP FUNCTION
	}
}
void putdown( int i){
	SDL_LockMutex (mutex);
	state[i] = THINKING;
	check (LEFT);
	check (RIGHT);
	SDL_UnlockMutex (mutex); 
}

void pickup (int i){
	//LockMutex is the same as having the mutex wait
	//Unlock is signal
	SDL_LockMutex (mutex); //philosopher gets lock from mutex
	state[i] = HUNGRY; //then sets their state to hungry
	//goes to check function
	check(i); //checks to see if left and right chopsticks are free
	SDL_UnlockMutex (mutex);
	//wait
	SDL_SemWait (s[i]); //blocks if chopsticks were not acquired
}

void put_chops( int i){
//STARTS HERE
	SDL_Delay( rand() % 500);
}

int philosopher(void *data){
	//STARTS HERE
	//infinite loop
	int i = atoi ( (char *) data);
	printf("\nPhilosopher %d", i );
	while( !quit){
		think(i);
		pickup(i);
		eat(i);
		putdown(i);
	}
}

void print_info(){
//prints out info of philosopher state
//checks how many philosophers are eating
	int n=0, a[2];
	SDL_LockMutex (mutex);
	for ( int i=0; i<5; i++)
		if (state[i] ==EATING)
			//if philosopher is eating we save information in array a
			a[n++] =i;
	SDL_UnlockMutex (mutex);
	//print out who is eating
	printf("\n# of philosophers eating is %d: ", n);
	for (int k = 0; k < n; k++){
		printf("%d", a[k] );
		if (k < n-1)
			printf(", ");
		}
	printf("\n");
}


//Thread showing information
int info (void *data){
	while (!quit) {
		SDL_Delay(1000); //1 second
		//prints out info every one second
		print_info();
	}
}

void checkCount (int sig){
	//when we type control c prints out info
	//or when we type control / prints out info and all semaphores will be unlocked
	if ((sig = SIGINT)) {
		printf("--------------------\n");
		print_info();
		printf("--------------------\n");
	}
	else if((sig = SIGQUIT)){
		quit = true;
		printf("\nQuitting, please wait…\n");
		SDL_LockMutex (mutex);
		for (int i=0; i < 5; i++ ){
			//release any lock
			SDL_SemPost (s[i]);
			printf("\nUnlocked %d", i );
		}
		SDL_UnlockMutex (mutex);
}
}

int main(){
	//to quit press “control \”
	struct sigaction act, actq;
	
//check signals if we want to interrupt program
	act.sa_handler = checkCount;
	sigemptyset ( &act.sa_mask);
	sigaction(SIGINT, &act, 0);
	actq.sa_handler = checkCount;
	sigaction (SIGQUIT, &actq, 0);

mutex = SDL_CreateMutex();
if(mutex == NULL){
printf("\n Mutex creation failed"); 
return 1;
}
SDL_Thread *p[5];    //thread identifiers
const char *names[]={ "0", "1", "2", "3", "4" };
SDL_Thread *infot;
for (int i=0; i < 5; i++)
 	s[i] = SDL_CreateSemaphore(0);
for (int i =0; i<5; i++)
	p[i] =SDL_CreateThread (philosopher, (char *) names[i] );

infot = SDL_CreateThread (info, NULL);
for ( int i =0; i<5; i++) 
	SDL_WaitThread(p[i], NULL);
SDL_WaitThread( infot, NULL);

for (int i=0; i< 5; i++)
	SDL_DestroySemaphore(s[i]);
SDL_DestroyMutex (mutex);
return 0;
}
