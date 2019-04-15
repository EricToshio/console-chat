#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <ctime>


// -----------------------------------------
// Configuracoes
// -----------------------------------------

// Configiracao do teste
#define NUMBER_OF_CLIENTS 2
#define NUMBER_OF_MESSAGES_FOR_CLIENT 100

// Configuracao do servidor
#define MAX_NUMBER_OF_CLIENTS 1000
#define SYNCHRONISM true

// Funcoes e variaveis das threads
pthread_t threadId[MAX_NUMBER_OF_CLIENTS+2];
sem_t mutex; 
sem_t mutex_auditor;
void up(sem_t *sem) {sem_post(sem);}
void down(sem_t *sem) {sem_wait(sem);}


int messagesSent[NUMBER_OF_CLIENTS];
int index_available;

// -----------------------------------------
// lista ligada de mensagens 
// -----------------------------------------
int created_blocks;
struct noh {
	int id;
	std::string message;
	struct noh* next;
} head;

struct noh* last;

struct noh* add_noh(int id, std::string message, struct noh* last_noh) {
	struct noh* new_noh = new noh;
	new_noh->next = NULL;
	new_noh->id = id;
	new_noh->message = message;
	if(last_noh != last){
		printf("<<<<<<<<<<<<<>>>>>>>>>>>>\n");
		printf("usuario %d usou uma versao desatualizada da lista\n",id);
	}
	last_noh->next = new_noh;
	last_noh = new_noh;
	created_blocks++;
	return last_noh;
}
// -----------------------------------------
// Classe servidor
// -----------------------------------------
class Server
{
public:
	
	Server();
	void test();
	void infinite_execution();
	// Fucoes das threads
	static void *auditor( void *arg );
	static void *client(void *arg );
	static void *client_send_message(void *arg );
	static void *server_console_control(void *arg);
	
};
// -----------------------------------------
// MAIN
// -----------------------------------------
int main(int argc, char const *argv[])
{
	Server server;
	server.test();
	return 0;
}
// -----------------------------------------
// Funcoes da classe servidor
// -----------------------------------------
// Construtor
Server::Server()
{	
	index_available = 0;
	created_blocks = 0;
	head.next = NULL;
	last = &head;
	sem_init (&mutex, 0, 1);
	sem_init (&mutex_auditor,0,1);
}

// Teste para comparação com o sequencial
void Server::test()
{
	int i;
	time_t time_atual;

	time_atual = time(NULL);

	for(i=0;i<NUMBER_OF_CLIENTS;i++) {
		pthread_create (&threadId[i],NULL,client,NULL);
	}
	for(i=0;i<NUMBER_OF_CLIENTS;i++) {
		pthread_join (threadId[i], NULL);		
	}

	time_atual = time(NULL)- time_atual;
	std::cout << "Demorou " << time_atual << " segundos" << std::endl;
}

void Server::infinite_execution(){

	int i;

	for(i=0;i<NUMBER_OF_CLIENTS;i++) {
		pthread_create (&threadId[i],NULL,client,NULL);
	}

	pthread_create (&threadId[NUMBER_OF_CLIENTS],NULL,auditor,NULL);	

	pthread_create (&threadId[NUMBER_OF_CLIENTS+1],NULL,server_console_control,NULL);


	for(i=0;i<NUMBER_OF_CLIENTS+1;i++) {
		pthread_join (threadId[i], NULL);		
	}
}

void* Server::server_console_control(void *arg)
{
	std::string command;
	while(true){
		std::cin >> command;
		
		if(command == "help"){
			std::cout << "Comandos que podem ser executado" << std::endl;
			std::cout << "show: mostra situacao atual do servidor" << std::endl;
		}else if (command == "show"){
			//pthread_create (&threadId[NUMBER_OF_CLIENTS],NULL,auditor,NULL);	
		}else{
			std::cout << "Comando nao encontrado, para comandos validos digite \"help\"" << std::endl;
		}
	}
 	
}


void* Server::auditor( void *arg ) {
	printf("Auditor online\n");
	int cont[NUMBER_OF_CLIENTS];
	int i;
	int blocks;

	while(true) {
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
	}
	
}

void* Server::client( void *arg) {
	int id = (int) pthread_self();
	int index;

	if(SYNCHRONISM)
		down(&mutex);
	  index = index_available++;
	  messagesSent[index] = 0;
	if(SYNCHRONISM)
		up(&mutex);

	std::cout << "Usuario " << id << " conectado com indice " << index << std::endl;
	std::string message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla auctor eu erat ut porta. In feugiat turpis auctor elit laoreet accumsan. Interdum et malesuada fames ac ante ipsum primis in faucibus.";
	while(true) {
		sleep(1);
		if(SYNCHRONISM)
			down(&mutex);
		last = add_noh(index, message, last);
		messagesSent[index]++;
		if(SYNCHRONISM)
			up(&mutex);
	}
	return 0;
}

void* Server::client_send_message( void *arg) {
	int id = (int) pthread_self();
	int index;
	

	if(SYNCHRONISM)
		down(&mutex);
	  index = index_available++;
	  messagesSent[index] = 0;
	if(SYNCHRONISM)
		up(&mutex);

	//std::cout << "Usuario " << id << " conectado com indice " << index << std::endl;
	std::string message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla auctor eu erat ut porta. In feugiat turpis auctor elit laoreet accumsan. Interdum et malesuada fames ac ante ipsum primis in faucibus.";
	for(int i = 0; i < NUMBER_OF_MESSAGES_FOR_CLIENT; i++) {
		//sleep(1);
		if(SYNCHRONISM)
			down(&mutex);
		last = add_noh(index, message, last);
		messagesSent[index]++;
		if(SYNCHRONISM)
			up(&mutex);
	}

	return NULL;
}


