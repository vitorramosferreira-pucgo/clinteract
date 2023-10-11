//libsocket-client, biblioteca para composicao do socket ipv4. - Vitor Ramos Ferreira
//--metodos--
#include "libsocket.h"

int criarSocketIpv4Tcp() { return socket(AF_INET, SOCK_STREAM, 0); }

struct sockaddr_in* criarEnderecoIpv4(char *ip, int porta) {
    struct sockaddr_in *endereco = malloc(sizeof(struct sockaddr_in));
    endereco->sin_family = AF_INET;
    endereco->sin_port = htons(porta);
    inet_pton(AF_INET,ip,&endereco->sin_addr.s_addr);
    return endereco;
}