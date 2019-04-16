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
#define SECONDS_BETWEEN_MESSAGES 1
#define NUMBER_OF_MESSAGES_FOR_CLIENT 100

// -----------------------------------------
// lista ligada de mensagens 
// -----------------------------------------
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
	// Avisa caso ocorra incoerencia no historico de mensagens
	if(last_noh != last){
		printf("<<<<<<<<<<<<<>>>>>>>>>>>>\n");
		printf("Usuario %d usou uma versao desatualizada da lista de mensagens\n",id);
		printf("<<<<<<<<<<<<<>>>>>>>>>>>>\n");
	}
	last_noh->next = new_noh;
	last_noh = new_noh;

	return last_noh;
}
// -----------------------------------------
// Classe servidor
// -----------------------------------------
class Server
{
public:
	
	Server();
	// Executa o test
	void execute_test();
	// Faz o cliente adicionar uma mensagem ao historico
	void client_send_message(int index);
	
};
// -----------------------------------------
// MAIN
// -----------------------------------------
int main(int argc, char const *argv[])
{
	// Inicia o servidor
	Server server;
	// Cria o contador de tempo
	time_t time_atual;
	time_atual = time(NULL);
	// Executa o teste
	server.execute_test();
	// Calcula o tempo que demorou
	time_atual = time(NULL)- time_atual;
	// Analisa quantas mensagens foram criadas
	int number_messages = 0;
	struct noh* per = head.next;
	while(per != NULL){
		per = per->next;
		number_messages++;
	}
	// Apresenta o resutado no console
	std::cout << "Demorou " << time_atual << " segundos" << std::endl;
	std::cout << "Existem " << number_messages << " mensagens na lista" << std::endl;
	return 0;
}
// -----------------------------------------
// Funcoes da classe servidor
// -----------------------------------------

Server::Server()
{	
	head.next = NULL;
	last = &head;
}

void Server::execute_test()
{
	for (int i = 0; i < NUMBER_OF_MESSAGES_FOR_CLIENT; ++i){
		for(int j = 0; j<NUMBER_OF_CLIENTS; ++j) {
			client_send_message(j);
		}
	}
}

void Server::client_send_message(int index) {

	std::string message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla auctor eu erat ut porta. In feugiat turpis auctor elit laoreet accumsan. Interdum et malesuada fames ac ante ipsum primis in faucibus.";
	sleep(SECONDS_BETWEEN_MESSAGES);
	last = add_noh(index, message, last);
}


