//CLInteract-servidor - Vitor Ramos Ferreira

//para compilar: gcc main.c libsocket.c -o CLInteract-servidor -pthread -w

// AQUI VOCE PODE DEFINIR O NUMERO DE CLIENTES QUE PODEM
// CONECTAR NO SERVIDOR
#define MAX_CLIENTES 20

#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
//BIBLIOTECA UTILIZADA PARA DEFINICAO DO PADRAO SOCKET. VER ARQUIVO -->> libsocket.c e libsocket.h
#include "libsocket.h"

// ESTRUTURA UTILIZADA PARA RECEBER O DATAGRAMA DO CLIENTE

struct socketRemoto
{
    int socketRemotoDesc;
    struct sockaddr_in endereco;
    int erro;
    bool conexaoAceita;
};

struct socketRemoto clientes[MAX_CLIENTES];
int pos_cliente = 0;

//FUNCOES

struct socketRemoto * aceitarConexaoRemota(int socketServidorDesc); 

void receberMensagemNoServidor(int socketClienteDesc); 

void receberConexaoRemota(int socketServidorDesc);

void threadProcessaDadosConexaoRemota(struct socketRemoto *socketCliente);

void broadcastEnvioMensagem(char *mensagem, int socketClienteDesc);

//MAIN----------------------------------------------------------------------

int main() {

    // CRIACAO DO DATAGRAMA E DEFINICAO DO DESCRITOR DE ARQUIVO DO SERVIDOR
    int socketServidorDesc = criarSocketIpv4Tcp();

    // PEDE AO SISTEMA OPERACIONAL A ALOCACAO DE IP E PORTA SELECIONADA
    // E A GUARDA EM UMA ESTRUTURA DE TIPO SOCKET IPV4
    struct sockaddr_in *enderecoServidor = criarEnderecoIpv4("", 2000);

    // MAPEIA O SOCKET CONFIGURADO, AO DESCRITOR DE ARQUIVO CRIADO PARA O SERVIDOR
    int resultado = bind(socketServidorDesc, enderecoServidor, sizeof (*enderecoServidor));

    if(resultado == 0)
        printf("\nCLInteract 1.0 (Servidor) - (Terminar Processo: Ctrl+C)\n");
    // INICIA O PROCESSO DE ESCUTA PARA O DESCRITOR DE ARQUIVO SELECIONADO
    listen(socketServidorDesc, 20);
    
    // RECEBE AS SOLICITACOES DE CONEXAO  VER--->>receberConexaoRemota
    receberConexaoRemota(socketServidorDesc);

    shutdown(socketServidorDesc, SHUT_RDWR);

    return 0;
}

//--------------------------------------------------------------------------

void receberConexaoRemota(int socketServidorDesc) { 

    while(true)
    {
        // PARA CADA CONEXAO RECEBIDA, CRIA SE UMA ESTRUTURA DE RECEBIMENTO, E
        // ACEITA A CONEXAO, RECEBENDO O DATAGRAMA DO CLIENTE NO SERVIDOR
        // VER-->> aceitarConexaoRemota
        struct socketRemoto* socketCliente = aceitarConexaoRemota(socketServidorDesc);
        clientes[pos_cliente++] = *socketCliente;
        
        // ACEITA A CONEXAO, O DATAGRAMA DO CLIENTE ESTÁ NO SERVIDOR.
        // CADA CLIENTE PODE ENVIAR MULTIPLAS MENSAGENS, E O SERVIDOR PRECISA
        // ESCUTAR TODAS AS MENSAGENS DE FORMA SIMULTANEA, SENDO ASSIM, NECESSARIO 
        // SEPARAR CADA CLIENTE EM UMA THREAD. VER--> threadProcessaDadosConexaoRemota 
        threadProcessaDadosConexaoRemota(socketCliente);
    }

}

struct socketRemoto * aceitarConexaoRemota(int socketServidorDesc) {

    // CRIA UM SOCKET IPV4 VAZIO, ACEITA A CONEXAO E RECEBE O DATAGRAMA,
    // SEGUIDO DO DESCRITOR DE ARQUIVO DO CLIENTE 
    struct sockaddr_in enderecoCliente;
    int tamanhoEnderecoCliente = sizeof (struct sockaddr_in);
    int socketClienteDesc = accept(socketServidorDesc,&enderecoCliente,&tamanhoEnderecoCliente);

    // GRAVA AS INFORMACOES REMOTAS DO CLIENTE EM UMA ESTRUTURA TEMPORÁRIA
    // PARA FACILITAR O REFERENCIAMENTO DAS INFORMACOES
    struct socketRemoto* socketInstanciaCliente = malloc(sizeof(struct socketRemoto));
    socketInstanciaCliente->endereco = enderecoCliente;
    socketInstanciaCliente->socketRemotoDesc = socketClienteDesc;
    socketInstanciaCliente->conexaoAceita = socketClienteDesc > 0;

    if(!socketInstanciaCliente->conexaoAceita)
        socketInstanciaCliente->erro = socketClienteDesc;

    // RETORNA OS DADOS DO CLIENTE
    // VOLTAR --> receberConexaoRemota
    return socketInstanciaCliente;
}

void threadProcessaDadosConexaoRemota(struct socketRemoto *socketCliente) {

    // ABRE UM PROCESSO NO SISTEMA OPERACIONAL PARA CADA CLIENTE, FORNECE UM ID
    // E EXECUTA UMA ROTINA, QUE É DESEMPACOTAR A MENSAGEM DO DATAGRAMA PARA
    // SER VISUALIZADA E PROCESSADA POSTERIORMENTE. VER-->> receberMensagemNoServidor
    pthread_t id;
    pthread_create(&id, NULL, receberMensagemNoServidor, socketCliente->socketRemotoDesc);
}

void receberMensagemNoServidor(int socketClienteDesc) {

    // A PARTIR DAQUI O CLIENTE ESTÁ EM UM PROCESSO SEPARADO, E CONEXAO ESTABELECIDA, 
    // ENQUANTO O SERVIDOR PODE OUVIR E RECEBER MENSAGEM DE OUTROS CLIENTES.
 
    // A MENSAGEM É DEFINIDA COMO UM CONJUNTO DE 1024 CARACTERES, PODENDO SER ALTERADO
    // PARA QUALQUER TAMANHO.
    char mensagem[1024];

    while(true)
    {
        // EXTRAI O CAMPO "MENSAGEM" DO DATAGRAMA DO CLIENTE, COM O TAMANHO ESPECIFICADO
        ssize_t tamanhoMensagem = recv(socketClienteDesc, mensagem, 1024, 0);

        if(tamanhoMensagem > 0)
        {
            // SE MENSAGEM FOR NAO NULA, IMPRIME A MENSAGEM NO SERVIDOR.
            mensagem[tamanhoMensagem] = 0;
            printf("%s\n", mensagem);
            // EM SEGUIDA, REALIZA O ENVIO PARA OS OUTROS CLIENTES.
            // VER --> broadcastEnvioMensagem
            broadcastEnvioMensagem(mensagem, socketClienteDesc);
        }
        if(tamanhoMensagem == 0)
            break;
    }
    // CASO O CLIENTE FECHE O TERMINAL, A CONEXAO PARA O CLIENTE É FINALIZADA.
    close(socketClienteDesc);
}

void broadcastEnvioMensagem(char *mensagem, int socketClienteDesc) {

    // EM receberConexaoRemota, GUARDA SE OS DADOS DE CADA CLIENTE, COM O INTUITO DO
    // SERVIDOR REALIZAR O BROADCAST COM CADA CLIENTE.

    // PARA CADA CLIENTE DENTRO DA LISTA ATÉ O LIMITE MÁXIMO DE CLIENTES,
    // A MENSAGEM E REPLICADA EM CADA CLIENTE.
    for(int i = 0; i < pos_cliente; i++)
        if(clientes[i].socketRemotoDesc != socketClienteDesc)
        {
            send(clientes[i].socketRemotoDesc, mensagem, strlen(mensagem), 0);
        }

}

