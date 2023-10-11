//libsocket-client, biblioteca para composicao do socket ipv4. - Vitor Ramos Ferreira
//--cabecalho--
#ifndef CLIENTE_LIBSOCKET_H
#define CLIENTE_LIBSOCKET_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>

struct sockaddr_in* criarEnderecoIpv4(char *ip, int porta);

int criarSocketIpv4Tcp();

#endif //CLIENTE_LIBSOCKET_H
