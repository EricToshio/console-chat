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
	static void client_send_message(int index);
	
};
// -----------------------------------------
// MAIN
// -----------------------------------------
int main(int argc, char const *argv[])
{
	time_t time_atual;

	time_atual = time(NULL);

	Server server;

	time_atual = time(NULL)- time_atual;
	std::cout << "Demorou " << time_atual << " segundos" << std::endl;
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
	

	for (int i = 0; i < NUMBER_OF_MESSAGES_FOR_CLIENT; ++i){
		for(int j = 0; j<NUMBER_OF_CLIENTS; ++j) {
			client_send_message(j);
		}
	}

}

void Server::client_send_message(int index) {

	//std::cout << "Usuario " << id << " conectado com indice " << index << std::endl;
	std::string message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla auctor eu erat ut porta. In feugiat turpis auctor elit laoreet accumsan. Interdum et malesuada fames ac ante ipsum primis in faucibus.";
	sleep(1);
	last = add_noh(index, message, last);
}


