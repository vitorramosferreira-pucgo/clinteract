//libsocket-server, biblioteca para composicao do endereco ipv4. - Vitor Ramos Ferreira

//--metodos--
#include "libsocket.h"

int criarSocketIpv4Tcp() { return socket(AF_INET, SOCK_STREAM, 0); }

struct sockaddr_in* criarEnderecoIpv4(char *ip, int porta) {
    struct sockaddr_in *endereco = malloc(sizeof(struct sockaddr_in));
    endereco->sin_family = AF_INET;
    endereco->sin_port = htons(porta);
    if(strlen(ip) == 0)
        endereco->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET,ip,&endereco->sin_addr.s_addr);
    return endereco;
}
