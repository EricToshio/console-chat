#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <iostream>
/* C++ 
#include <iostream>
#include <thread>        
#include <mutex>         
#include <condition_variable>
*/

sem_t full;
sem_t empty;
sem_t mutex;

using namespace std;

int main(int argc, char const *argv[])
{
	int i;

	last_produced_item = 0;
	start = 0;
	end = 0;

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

	return 0;
}