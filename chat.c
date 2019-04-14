#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define TRUE 1
#define FALSE 0

#define NUMBER_OF_CLIENTS 2
#define BUG_VERSION TRUE

#define N 10
#define WINAPI
#define debugtxt(FORMAT) printf(" TID %d: " #FORMAT "\n",(int) pthread_self())
#define debug(FORMAT, ARGS...) printf("TID %d: " #FORMAT "\n",(int) pthread_self(),ARGS)

typedef void *LPVOID;


// lista ligada
// -----------------------------------------
int created_blocks;
struct noh {
	int id;
	char message[100];
	struct noh* next;
} head;

struct noh* last;

struct noh* add_noh(int id, char* message, struct noh* last_noh) {
	struct noh* new = (struct noh*)malloc(sizeof(struct noh));
	new->next = NULL;
	new->id = id;
	strcpy(new->message,message);
	if(last_noh != last){
		printf("<<<<<<<<<<<<<>>>>>>>>>>>>\n");
		printf("usuario %d usou uma versao desatualizada da lista\n",id);
	}
	last_noh->next = new;
	last_noh = new;
	created_blocks++;
	return last_noh;
}
// -----------------------------------------


// Funcoes e variaveis das threads
pthread_cond_t handleThread[NUMBER_OF_CLIENTS+1];
pthread_t threadId[NUMBER_OF_CLIENTS+1];

sem_t mutex; //pthread_cond_t mutex;
sem_t mutex_auditor;

// Truque para sabermos qual o semaforo foi chamado e poder imprimi-lo
#define up(SEM) _up(SEM,#SEM)
#define down(SEM) _down(SEM,#SEM)

void _up(sem_t *sem, const char * name) {
	// debug("Up %s ...",name);
	sem_post(sem);
	// debug("Up %s complete!",name);
}
void _down(sem_t *sem, const char * name) {
	// debug("Down %s ...",name);
	sem_wait(sem);
	// debug("Down %s complete!",name);
}

int messagesSent[NUMBER_OF_CLIENTS];
int index_available;

unsigned long WINAPI client( LPVOID lpParam ) {
	int id = (int) pthread_self();
	int index;
	down(&mutex);
	  index = index_available++;
	  messagesSent[index] = 0;
	up(&mutex);
	printf("Usuario %d conectado com indice %d\n", id,index);
	char* message = "usuario 1 diz: hello world!";
	while(TRUE) {
		sleep(1);
		down(&mutex_auditor);
		up(&mutex_auditor);
		if(!BUG_VERSION)
			down(&mutex);
		last = add_noh(index, message, last);
		messagesSent[index]++;
		if(!BUG_VERSION)
			up(&mutex);
	}
	return 0;
}

unsigned long WINAPI auditor( LPVOID lpParam ) {
	printf("Auditor online\n");
	int cont[NUMBER_OF_CLIENTS];
	int i;
	int blocks;
	while(TRUE) {
		sleep(3);
		down(&mutex_auditor);
		down(&mutex);
		for(i=0;i<NUMBER_OF_CLIENTS;i++) cont[i] = 0;
		blocks = 0;
		struct noh* per = head.next;
		while(per != NULL){
			cont[per->id]++;
			per = per->next;
			blocks++;
		}
		printf("-----------------------auditoria-----------------------------\n");
		printf("Auditoria:\n");
		for(i=0;i<NUMBER_OF_CLIENTS;i++){
			printf("mensagens que o usuario %d diz ter enviado: %d\n",i, messagesSent[i]);
			printf("mensagens que a auditoria encontrou      : %d\n", cont[i]);
		}
		printf("blocos que foram criados : %d\n",created_blocks);
		printf("blocos presentes na lista: %d\n",blocks);
		printf("--------------------------fim--------------------------------\n");
		
		up(&mutex);
		up(&mutex_auditor);
		// up(&mutex);
	}
	return 0;
}

int main() {
	index_available = 0;
	created_blocks = 0;
	head.next = NULL;
	last = &head;
	sem_init (&mutex, 0, 1);
	sem_init (&mutex_auditor,0,1);
	
	int i;
	void* threadFunc[NUMBER_OF_CLIENTS+1];
	for(i=0;i<NUMBER_OF_CLIENTS;i++) threadFunc[i] = client;
	threadFunc[NUMBER_OF_CLIENTS] = auditor;
	for(i=0;i<NUMBER_OF_CLIENTS;i++) {
		pthread_create (&threadId[i],
				NULL,
				threadFunc[i],
				NULL);
	}
	pthread_create (&threadId[NUMBER_OF_CLIENTS],
				NULL,
				threadFunc[NUMBER_OF_CLIENTS],
				NULL);

	for(i=0;i<NUMBER_OF_CLIENTS+1;i++) {
		pthread_join (threadId[i], NULL);		
	}
	
}