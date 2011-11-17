/*
 * PPD-Consola.c
 *
 *  Created on: 26/10/2011
 *      Author: Martin Silberkasten
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/un.h>
#include <sys/socket.h>


#include "socketsUnix.h"
#include "list.h"
#include "array.h"
#include "scanner.h"
#include "commands.h"

#define SOCK_PATH "socket_consola"

t_socket_unix_client *client;

void funcInfo(void *nada, t_array *nada2){
	sockets_unix_sendString(client,"funcInfoPPD");

	printf("Posicion actual: \n");
}
void funcClean(void *nada, t_array *clusters){
	//NO FUNCIONA ARRAY_GET y la "!#$%$&/%$/
	int *clusterInicial, *clusterFinal;
	clusterInicial = array_get(clusters,1);
	clusterFinal = array_get(clusters,2);
	printf("Cluster inicial: %d\n",clusters->data[0]);
	printf("Cluster final: %d\n",clusters->data[1]);
}
void funcTrace(void *nada, t_array *nada2){
	printf("Posicion actual:");
}

int main(int argc,char *argv[]){
	/*int s, len;
	struct sockaddr_un local;*/
	char input[20];


	t_commands *comandos = commands_create("-", "", (int)" ");
	commands_add(comandos, "info",funcInfo);
	commands_add(comandos, "clean",funcClean);
	commands_add(comandos, "trace",funcTrace);

	//printf("** Estableciendo conexion **\n\n");

	client= sockets_unix_createClient();
	while(!sockets_unix_connect(client)){
		sleep(1);
	};

	//fprintf(stdout,"\033[2J");
	//fprintf(stdout,"\033[1;1H");
	//printf("** Welcome back master **\n\n");
	sockets_unix_sendString(client,"Luke i'm you client");



	while(1){
		scanf("%s",input);
		commands_parser(comandos,input);
		sleep(1);
	}
	sockets_unix_destroyClient(client);
	return 0;
}
