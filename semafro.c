#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0
#define N 10
#define WINAPI
#define debugtxt(FORMAT) printf(" TID %d: " #FORMAT "\n",(int) pthread_self())
#define debug(FORMAT, ARGS...) printf("TID %d: " #FORMAT "\n",(int) pthread_self(),ARGS)

typedef void *LPVOID;

// Funcoes e variaveis do buffer
int start;
int end;
int buffer[N];
int count;
int last_produced_item;
int last_consumed_item;

void insert_item(int item) {
	debug("Inserting item %d",item);
	buffer[end]=item;
	end=(end+1)%N;
	debugtxt("Item inserted!");
}
int remove_item() {
	debugtxt("Removing item ...");
	int item = buffer[start];
	start=(start+1)%N;
	debug("Item %d removed!",item);
	return item;
}

// Funcoes e variaveis das threads
pthread_cond_t handleThread[2];
pthread_t threadId[2];
const int producer = 0;
const int consumer = 1;
sem_t full;
sem_t empty;
sem_t mutex; //pthread_cond_t mutex;

// Truque para sabermos qual o semaforo foi chamado e poder imprimi-lo
#define up(SEM) _up(SEM,#SEM)
#define down(SEM) _down(SEM,#SEM)

void _up(sem_t *sem, const char * name) {
	debug("Up %s ...",name);
	sem_post(sem);
	debug("Up %s complete!",name);
}
void _down(sem_t *sem, const char * name) {
	debug("Down %s ...",name);
	sem_wait(sem);
	debug("Down %s complete!",name);
}

// Produtor e consumidor ...
int produce_item() {
	debugtxt("Producing item ...");
	last_produced_item++;
	debug("Produced item %d",last_produced_item);
	return last_produced_item;
}
void consume_item(int item) {
	debugtxt("Consuming item ...");
	last_consumed_item = item;
	debug("Consumed item %d",item);
}

unsigned long WINAPI producerFunc( LPVOID lpParam ) {
	debugtxt("Starting producer");
	int item;
	while(TRUE) {
		item=produce_item();
		down(&empty);
		down(&mutex);
		int j = 0;
		for (; j < 1000000; j++);
		insert_item(item);
		up(&mutex);
		up(&full);
	}
	debugtxt("Ending producer");
	return 0;
}

unsigned long WINAPI consumerFunc( LPVOID lpParam ) {
	debugtxt("Starting consumer");
	int item;
	while(TRUE) {
		down(&full);
		down(&mutex);
		item = remove_item();
		up(&mutex);
		up(&empty);
		consume_item(item);
	}
	debugtxt("Ending consumer");
	return 0;
}

int main() {
	int i;

	last_produced_item = 0;
	start = 0;
	end = 0;
	// Criando semaforos ...
	/*full = CreateSemaphore( 
			NULL,           // default security attributes
			0,			// initial count
			N,  			// maximum count
			NULL);
	empty = CreateSemaphore( 
			NULL,           // default security attributes
			N,			// initial count
			N,  			// maximum count
			NULL);
	mutex = CreateSemaphore( 
			NULL,           // default security attributes
			1,			// initial count
			1,  			// maximum count
			NULL);*/
	sem_init (&full, 0, 0);
	sem_init (&empty, 0, N);
	sem_init (&mutex, 0, 1);
	
	void *threadFunc[2] = { producerFunc, consumerFunc };

	   for(i=0;i<2;i++) {
			pthread_create (&threadId[i],
					NULL,
					threadFunc[i],
					NULL);
	   }

	   for(i=0;i<2;i++) {
			pthread_join (threadId[i], NULL);		
	   }
	
}