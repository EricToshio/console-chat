#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define N 10
#define WINAPI
#define debugtxt(FORMAT) printf(" TID %d: " #FORMAT "\n",(int) pthread_self())
#define debug(FORMAT, ARGS...) printf("TID %d: " #FORMAT "\n",(int) pthread_self(),ARGS)

typedef void *LPVOID;


// lista ligada
// -----------------------------------------
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
		printf("<<<<<<<<<< mensagem perdida >>>>>>>>>>>>\n");
	}
	last_noh->next = new;
	last_noh = new;
	return last_noh;
}
// -----------------------------------------


// Funcoes e variaveis das threads
pthread_cond_t handleThread[2];
pthread_t threadId[2];

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

int messagesSent1, messagesSent2;

unsigned long WINAPI client1( LPVOID lpParam ) {
	debugtxt("Usuario 1 conectado");
	char* message = "usuario 1 diz: hello world!";
	int id = 1;
	while(TRUE) {
		sleep(1);
		down(&mutex_auditor);
		up(&mutex_auditor);
			// down(&mutex);
		last = add_noh(id, message, last);
		messagesSent1++;
			// up(&mutex);
	}
	// debugtxt("Ending producer");
	return 0;
}

unsigned long WINAPI client2( LPVOID lpParam ) {
	debugtxt("Usuario 2 conectado");
	char* message = "usuario 2 diz: hello world!";
	int id = 2;
	while(TRUE) {
		sleep(1);
		down(&mutex_auditor);
		up(&mutex_auditor);
			// down(&mutex);
		last = add_noh(id, message, last);
		messagesSent2++;
			// up(&mutex);
	}
	debugtxt("Ending producer");
	return 0;
}

unsigned long WINAPI auditor( LPVOID lpParam ) {
	debugtxt("Auditor online");
	int cont1 = 0;
	int cont2 = 0;
	while(TRUE) {
		sleep(3);
		down(&mutex_auditor);
		down(&mutex);
		struct noh* per = head.next;
		while(per != NULL){
			if(per->id == 1)
				cont1++;
			else
				cont2++;
			per = per->next;
		}
		printf("-------------------------------------------------------\n");
		printf("Auditoria:\n");
		printf("mensagens que o usuario 1 diz ter enviado: %d\n", messagesSent1);
		printf("mensagens que a auditoria encontrou      : %d\n", cont1);
		printf("mensagens que o usuario 2 diz ter enviado: %d\n", messagesSent2);
		printf("mensagens que a auditoria encontrou      : %d\n", cont2);
		cont1 = 0;
		cont2 = 0;
		up(&mutex);
		up(&mutex_auditor);
		// up(&mutex);
	}
	debugtxt("Ending producer");
	return 0;
}

int main() {
	messagesSent1 = 0;
	messagesSent1 = 0;
	head.next = NULL;
	last = &head;
	sem_init (&mutex, 0, 1);
	sem_init (&mutex_auditor,0,1);
	
	void *threadFunc[3] = { client1, client2, auditor };
	int i;
	   for(i=0;i<3;i++) {
			pthread_create (&threadId[i],
					NULL,
					threadFunc[i],
					NULL);
	   }

	   for(i=0;i<2;i++) {
			pthread_join (threadId[i], NULL);		
	   }
	
}