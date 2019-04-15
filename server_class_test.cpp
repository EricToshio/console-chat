#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>

#define NUMBER_OF_CLIENTS 2
#define synchronism false
typedef void *LPVOID;

// semafro
sem_t mutex;


// Classe servidor
class Server
{
public:

	pthread_t threadId;
	
	// informações do chat
	std::string chat_data;

	Server();
	//~Server();
	void init();

	static void *client_receive(void *vargp);
	static void *server_console_control(void *vargp);
	
};

// Construtor
Server::Server()
{	
	// configurando mutex 
	sem_init (&mutex, 0, 1);
	
	// inicializando informações do chat_data
	chat_data = "Bem-vindo ao console-chat!\n";

	//init();
	// thread de controle
	pthread_create (&threadId, NULL, server_console_control, NULL);
	pthread_create (&threadId, NULL, client_receive, NULL);
	pthread_join (threadId, NULL);
	std::cout<< "aki foi" << std::endl;
}


void* Server::client_receive(void *vargp)
{
	while(true){
		sem_wait(&mutex);
		for(int i =0; i<100;i++){
		std::cout<< "hello222" << std::endl;
		//myfile << "hello222\n";
		sem_post(&mutex);
		//sleep(1);
		}
	}
 	
}
void* Server::server_console_control(void *vargp)
{
	std::string aki;
	while(true){
		//sem_wait(&mutex);
		//std::cin >> aki;
		//std::cout << "Recebi: " << aki << std::endl;
		//sem_post(&mutex);
		//sleep(3);
		sem_wait(&mutex);
		for(int i =0; i<100;i++){
		std::cout<< "hello111" << std::endl;
		//myfile << "hello111\n";
		sem_post(&mutex);
		//sleep(1);
		}
	}
 	
}

void Server::init(){
	std::cout << "finaliza" << std::endl;
}



int main(int argc, char const *argv[])
{
	Server server;
	return 0;
}