/*
 * socketsUnix.c
 *
 *  Created on: 01/11/2011
 *      Author: utn_so
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/un.h>

#include "socketsUnix.h"
#include "sockets.h"

#define SOCK_PATH "socket_consola"

t_socket_unix *sockets_unix_create(){
	t_socket_unix* sckt = malloc( sizeof(t_socket_unix) );
	sckt->desc = socket(AF_UNIX, SOCK_STREAM, 0);

	if(sckt->desc==-1){
		perror("Error en la creacion del socket");
		exit(1);
	}

	if( !sockets_unix_bind(sckt) ){
		free(sckt);
		return NULL;
	}
	sockets_unix_setMode(sckt, SOCKETMODE_BLOCK);
	return sckt;
}

int sockets_unix_bind(t_socket_unix* sckt){
	struct sockaddr_un *addr = malloc(sizeof(struct sockaddr_un));
	addr->sun_family = AF_UNIX;
	strcpy(addr->sun_path,SOCK_PATH);

	sckt->my_addr=addr;
	unlink(sckt->my_addr->sun_path);

	if(bind(sckt->desc,(struct sockaddr *)sckt->my_addr, sizeof(struct sockaddr_un))==-1){
		perror("Error en el bindeo");
		free(sckt->my_addr);
		sckt->my_addr = NULL;
		return 0;
	}
	return 1;
}

void sockets_unix_destroy(t_socket_unix* sckt){
	if( sckt->desc > 0 ){
		close(sckt->desc);
	}
	free(sckt->my_addr);
	free(sckt);
}

void sockets_unix_setMode(t_socket_unix *sckt, e_socket_mode mode){
	sckt->mode = mode;
}

e_socket_mode sockets_unix_getMode(t_socket_unix *sckt){
	return sckt->mode;
}

int sockets_unix_isBlocked(t_socket_unix *sckt){
	return sckt->mode == SOCKETMODE_BLOCK;
}

/*
 *						Client Socket Functions
 */

t_socket_unix_client *sockets_unix_createClient(){
	t_socket_unix_client* client = malloc( sizeof(t_socket_unix_client) );
	if( (client->socket = sockets_unix_create()) == NULL ){
		free(client);
		return NULL;
	}
	if (client->socket == NULL){
		return NULL;
	}
	sockets_unix_setState(client, SOCKETSTATE_DISCONNECTED);
	return client;
}

t_socket_unix *sockets_unix_getClientSocket(t_socket_unix_client *client){
	return client->socket;
}

int sockets_unix_equalsClients(t_socket_unix_client *client1, t_socket_unix_client *client2){
	return client1->socket->desc ==  client2->socket->desc;
}

int	sockets_unix_isConnected(t_socket_unix_client *client){
	return client->state == SOCKETSTATE_CONNECTED;
}

e_socket_state sockets_unix_getState(t_socket_unix_client *client){
	return client->state;
}

void sockets_unix_setState(t_socket_unix_client *client, e_socket_state state){
	client->state = state;
}

int sockets_unix_connect(t_socket_unix_client *client){
	struct sockaddr_un *addr = malloc(sizeof(struct sockaddr_un));
	t_socket_unix *serv_socket = malloc( sizeof(t_socket_unix) );

	addr->sun_family = AF_UNIX;
	strcpy(addr->sun_path,SOCK_PATH);

	serv_socket->my_addr=addr;
	//strcpy(serv_socket->my_addr->sun_path,SOCK_PATH);
	//serv_socket->my_addr->sun_family = AF_UNIX;

	if ( connect(client->socket->desc, (struct sockaddr *)serv_socket->my_addr , sizeof(struct sockaddr_un) ) == -1) {
		free(serv_socket->my_addr);
		free(serv_socket);
		client->serv_socket = NULL;
		return 0;
	}
	client->serv_socket = serv_socket;
	sockets_unix_setState(client, SOCKETSTATE_CONNECTED);
	return 1;
}

int sockets_unix_send(t_socket_unix_client *client, void *data, int datalen){
	int aux = send(client->socket->desc, data, datalen, 0);
	if( aux == -1 )
		sockets_unix_setState(client, SOCKETSTATE_DISCONNECTED);
	return aux;
}

int sockets_unix_sendBuffer(t_socket_unix_client *client, t_socket_buffer *buffer){
	return sockets_unix_send(client, buffer->data, buffer->size);
}

int sockets_unix_sendSBuffer(t_socket_unix_client *client, t_socket_sbuffer *buffer){
	return sockets_unix_send(client, buffer->serializated_data, buffer->size);
}

int sockets_unix_sendString(t_socket_unix_client *client, char *str){
	return sockets_unix_send(client, (void*)str, strlen(str)+1);
}

int sockets_unix_sendSerialized(t_socket_unix_client *client, void *data, t_socket_sbuffer *(*serializer)(void*)){
	t_socket_sbuffer *sbuffer = serializer(data);
	if( sbuffer != NULL){
		int ret = sockets_unix_send(client, (void*)sbuffer->serializated_data, sbuffer->size);
		free( sbuffer->serializated_data );
		free( sbuffer );
		return ret;
	}
	return -1;
}


void *sockets_unix_recvSerialized(t_socket_unix_client *client, void *(*deserializer)(t_socket_sbuffer*)){
	t_socket_buffer *buffer = sockets_unix_recv(client);
	void *data = NULL;

	if( buffer != NULL ){
		t_socket_sbuffer sbuffer;
		sbuffer.size = buffer->size;
		sbuffer.serializated_data = malloc(buffer->size);
		memcpy(sbuffer.serializated_data, buffer->data, buffer->size);
		data = deserializer(&sbuffer);
		free(sbuffer.serializated_data);
		free(buffer);
	}
	return data;
}

t_socket_buffer *sockets_unix_recv(t_socket_unix_client *client){
	t_socket_buffer *tbuffer = malloc( sizeof(t_socket_buffer) );
	int datasize = sockets_unix_recvInBuffer(client, tbuffer);

	if( datasize <= 0 ){
		free(tbuffer);
		return NULL;
	}

	return tbuffer;
}


int sockets_unix_recvInBuffer(t_socket_unix_client *client, t_socket_buffer *buffer){
	memset(buffer->data, 0, DEFAULT_BUFFER_SIZE);
	if( !sockets_unix_isBlocked(client->socket) ){
		fcntl(client->socket->desc, F_SETFL, O_NONBLOCK);
	}
	buffer->size = recv(client->socket->desc, buffer->data, DEFAULT_BUFFER_SIZE, 0);
	if( !sockets_unix_isBlocked(client->socket) ){
		fcntl(client->socket->desc, F_SETFL, O_NONBLOCK);
	}

	if( buffer->size == -1 )
		sockets_unix_setState(client, SOCKETSTATE_DISCONNECTED);

	return buffer->size;
}

void sockets_unix_destroyClient(t_socket_unix_client *client){
	if( !sockets_unix_isConnected(client) ) client->socket->desc = -1;
	sockets_unix_destroy(client->socket);
	free(client);
}

/*
 *						Server Socket Functions
 */

t_socket_unix_server *sockets_unix_createServer(char *ip, int port){
	t_socket_unix_server *sckt = malloc( sizeof(t_socket_unix_server) );
	if( (sckt->socket = sockets_unix_create()) == NULL ){
		free(sckt);
		return NULL;
	}
	sckt->maxconexions = DEFAULT_MAX_CONEXIONS;
	return sckt;
}

t_socket_unix *sockets_unix_getServerSocket(t_socket_unix_server* server){
	return server->socket;
}

void sockets_unix_setMaxConexions(t_socket_unix_server* server, int conexions){
	server->maxconexions = conexions;
}

int sockets_unix_getMaxConexions(t_socket_unix_server* server){
	return server->maxconexions;
}

int sockets_unix_listen(t_socket_unix_server* server){

	if (listen(server->socket->desc, server->maxconexions) == -1) {
		return 0;
	}
	return 1;
}

t_socket_unix_client *sockets_unix_accept(t_socket_unix_server* server){
	t_socket_unix_client* client = malloc( sizeof(t_socket_unix_client) );
	int addrlen = sizeof(struct sockaddr_un);
	client->socket = malloc( sizeof(t_socket_unix) );
	client->socket->my_addr = malloc( sizeof(struct sockaddr) );

	if( !sockets_unix_isBlocked(server->socket) ){
		fcntl(server->socket->desc, F_SETFL, O_NONBLOCK);
	}

	if ((client->socket->desc = accept(server->socket->desc, (struct sockaddr *)client->socket->my_addr, (void *)&addrlen)) == -1) {
		free(client->socket->my_addr);
		free(client->socket);
		free(client);
		return NULL;
	}

	if( !sockets_unix_isBlocked(server->socket) ){
		fcntl(server->socket->desc, F_SETFL, O_NONBLOCK);
	}

	sockets_unix_setState(client, SOCKETSTATE_CONNECTED);
	sockets_unix_setMode(client->socket, SOCKETMODE_BLOCK);
	return client;
}

/*
 * TODO En closure_clientsBuildSet hay que eliminar los sockets sockets_isConnected == 0
 */
void sockets_unix_select(t_list* servers, t_list* clients, int usec_timeout,  t_socket_unix_client *(*onAcceptClosure)(t_socket_unix_server*),  int (*onRecvClosure)(t_socket_unix_client*) ){
	fd_set conexions_set;
	int max_desc = 3;
	int select_return;
    struct timeval tv;
    t_list *close_clients = NULL;

    tv.tv_sec = 0;
    tv.tv_usec = usec_timeout;

    void closure_clientsBuildSet(t_socket_unix_client *client){
    	int curr_desc = client->socket->desc;
    	FD_SET(curr_desc, &conexions_set);
    	if( max_desc < curr_desc ) max_desc = curr_desc;
    }

    if( clients != NULL && collection_list_size(clients) > 0 ){
    	collection_list_iterator(clients, (void*)&closure_clientsBuildSet);
    }

    void closure_serversBuildSet(t_socket_unix_server *server){
    	int curr_desc = server->socket->desc;
    	FD_SET(curr_desc, &conexions_set);
    	if( max_desc < curr_desc ) max_desc = curr_desc;
    }

    if( servers != NULL && collection_list_size(servers) > 0 ){
    	collection_list_iterator(servers, (void*)&closure_serversBuildSet);
    }

    if( usec_timeout == 0 ){
    	select_return = select( max_desc + 1, &conexions_set, NULL, NULL, NULL );
    }else{
    	select_return = select( max_desc + 1, &conexions_set, NULL, NULL, &tv );
    }

    if( select_return == -1 ) return;

    void closure_acceptFromSocket(t_socket_unix_server *server){
    	if( FD_ISSET(server->socket->desc, &conexions_set) ){
    	   	t_socket_unix_client *auxClient = NULL;
    	   	if( &onAcceptClosure != NULL){
    	   		auxClient = onAcceptClosure(server);
    	   	}else if( clients != NULL ){
    	   		auxClient = sockets_unix_accept(server);
    	   	}

    	   	if( clients != NULL && auxClient != NULL){
    	   		collection_list_add(clients, auxClient);
    	   	}
    	}
    }
    if( servers != NULL && collection_list_size(servers) > 0 ){
    	collection_list_iterator(servers, (void*)&closure_acceptFromSocket);
    }

    void closure_recvFromSocket(t_socket_unix_client *client){
    	if( FD_ISSET(client->socket->desc, &conexions_set) && (&onRecvClosure != NULL) ){
    		if( onRecvClosure(client) == 0 ){
    			if( close_clients == NULL ) close_clients = collection_list_create();
    			collection_list_add(close_clients, client);
    		}
		}
    }
    if( clients != NULL && collection_list_size(clients) > 0 ){
    	collection_list_iterator(clients, (void*)&closure_recvFromSocket);
    	if( close_clients != NULL ){
    		int index;
    		for(index=0; index<collection_list_size(close_clients); index++ ){
    			t_socket_unix_client *client = collection_list_get(close_clients, index);
    			int closure_removeSocket(t_socket_unix_client *aux){ return client->socket->desc == aux->socket->desc; }
    			collection_list_removeByClosure(clients, (void*)&closure_removeSocket, NULL);
    		}
    		collection_list_destroy(close_clients, NULL);
    	}
    }
}

void sockets_unix_destroyServer(t_socket_unix_server* server){
	sockets_unix_destroy(server->socket);
	free(server);
}
