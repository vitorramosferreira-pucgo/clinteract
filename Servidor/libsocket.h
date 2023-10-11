//libsocket-server, biblioteca para composicao do endereco ipv4. - Vitor Ramos Ferreira

//--cabecalho--
#ifndef SERVIDOR_LIBSOCKET_H
#define SERVIDOR_LIBSOCKET_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>

struct sockaddr_in* criarEnderecoIpv4(char *ip, int porta);

int criarSocketIpv4Tcp();

#endif //SERVIDOR_LIBSOCKET_H
