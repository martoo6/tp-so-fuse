/*
 * socketsUnix.h
 *
 *  Created on: 01/11/2011
 *      Author: utn_so
 */
#include <sys/socket.h>
#include <sys/un.h>

#include "sockets.h"

#ifndef SOCKETSUNIX_H_
#define SOCKETSUNIX_H_

#define DEFAULT_BUFFER_SIZE 	2048
#define DEFAULT_MAX_CONEXIONS 	100
#define SELECT_USEC_TIMEOUT 	500

typedef struct {
	int desc;
	struct sockaddr_un *my_addr;
	e_socket_mode mode;
} t_socket_unix ;

typedef struct {
	t_socket_unix* socket;
	t_socket_unix* serv_socket;
	e_socket_state state;
} t_socket_unix_client ;

typedef struct {
	t_socket_unix    *socket;
	int			maxconexions;
} t_socket_unix_server ;

t_socket_unix *sockets_unix_create();
int sockets_unix_bind(t_socket_unix* sckt);
void sockets_unix_destroy(t_socket_unix* sckt);
void sockets_unix_setMode(t_socket_unix *sckt, e_socket_mode mode);

e_socket_mode    sockets_unix_getMode(t_socket_unix *sckt);
int              sockets_unix_isBlocked(t_socket_unix *sckt);

t_socket_unix_client *sockets_unix_createClient();
int				 sockets_unix_isConnected(t_socket_unix_client *client);
int              sockets_unix_equalsClients(t_socket_unix_client *client1, t_socket_unix_client *client2);
e_socket_state	 sockets_unix_getState(t_socket_unix_client *client);
void			 sockets_unix_setState(t_socket_unix_client *client, e_socket_state state);
int              sockets_unix_connect(t_socket_unix_client *client);
int              sockets_unix_send(t_socket_unix_client *client, void *data, int datalen);
int              sockets_unix_sendBuffer(t_socket_unix_client *client, t_socket_buffer *buffer);
int              sockets_unix_sendSBuffer(t_socket_unix_client *client, t_socket_sbuffer *buffer);
int              sockets_unix_sendString(t_socket_unix_client *client, char *str);
int              sockets_unix_sendSerialized(t_socket_unix_client *client, void *data, t_socket_sbuffer *(*serializer)(void*));
t_socket_buffer *sockets_unix_recv(t_socket_unix_client *client);
int 			 sockets_unix_recvInBuffer(t_socket_unix_client *client, t_socket_buffer *buffer);
int 			 sockets_unix_recvInSBuffer(t_socket_unix_client *client, t_socket_sbuffer *buffer);
void            *sockets_unix_recvSerialized(t_socket_unix_client *client, void *(*deserializer)(t_socket_sbuffer*));
void             sockets_unix_destroyClient(t_socket_unix_client *client);

t_socket_unix_server *sockets_unix_createServer();
void 			 sockets_unix_setMaxConexions(t_socket_unix_server* server, int conexions);
int              sockets_unix_getMaxConexions(t_socket_unix_server* server);
int 			 sockets_unix_listen(t_socket_unix_server* server);
t_socket_unix_client *sockets_unix_accept(t_socket_unix_server* server);
void 			 sockets_unix_select(t_list* servers,
								t_list *clients,
								int usec_timeout,
								t_socket_unix_client *(*onAcceptClosure)(t_socket_unix_server*),
								int (*onRecvClosure)(t_socket_unix_client*) );
void 			 sockets_unix_destroyServer(t_socket_unix_server* server);

#endif /* SOCKETSUNIX_H_ */
