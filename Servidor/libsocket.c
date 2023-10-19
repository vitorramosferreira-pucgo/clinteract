//libsocket-server, biblioteca para composicao do endereco ipv4. - Vitor Ramos Ferreira

//--metodos--
#include "libsocket.h"

// RETORNA UM PACOTE VAZIO, DA FAMILIA AF_INET (IPV4), TIPO SOCK_STREAM (TCP), E TIPO DE PROTOCOLO DEFAULT (0)
// ASSUME SE (0) COMO FORMA DE O SISTEMA DECIDIR O PROTOCOLO ADEQUADO DADO A FAMILIA DE ENDDERECOS E O O FORMATO
// DE PACOTE ESCOLHIDO, SEJA SOCK_STREAM (TCP) OU SOCK_DGRAM (UDP).
int criarSocketIpv4Tcp() { return socket(AF_INET, SOCK_STREAM, 0); }

// NESTA ETAPA E ESTABELECIDO A CRIACAO DA ESTRUTURA DE TRANSPORTE. RECEBE-SE UM ENDERECO E PORTA CONFIGURADO NO
// SERVIDOR, OU REPASSADO PELO USUARIO NO TERMINAL, E COVERTIDO PARA OS PADROES NECESSARIOS PARA A CAMADA DE TRANSPORTE.
struct sockaddr_in* criarEnderecoIpv4(char *ip, int porta) {
    struct sockaddr_in *endereco = malloc(sizeof(struct sockaddr_in));
    endereco->sin_family = AF_INET;
    endereco->sin_port = htons(porta);
    if(strlen(ip) == 0)
        // CASO NAO SEJA INFORMADO IP NO ATO DA COMPILACAO DO CODIGO, O SERVIDOR ASSUME EM UTILIZAR O IP CORRENTE
        // DA MAQUINA ATUAL E LOCALHOST 127.0.0.1 COMO DEFAULT. (INADDR_ANY)
        endereco->sin_addr.s_addr = INADDR_ANY;
    else
        // NESTE PONTO E FEITA A REPRESENTACAO BINARIA DA ESTRUTURA IPV4, E RETORNADO UM PONTEIRO AO CHAMADOR.        
        inet_pton(AF_INET,ip,&endereco->sin_addr.s_addr);
    return endereco;
}
