//CLInteract-cliente - Vitor Ramos Ferreira

//para compilar: gcc main.c libsocket.c -o CLInteract-cliente -pthread -w

#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
//BIBLIOTECA UTILIZADA PARA DEFINICAO DO PADRAO SOCKET. VER ARQUIVO -->> libsocket.c e libsocket.h
#include "libsocket.h"

//FUNCOES

void threadBroadcastReceberMensagem(int socketClienteDesc);

void receberMensagem(int socketClienteDesc);

//MAIN----------------------------------------------------------------------

int main(int argc, char *argv[]) {

    // LAYOUT DE ENTRADA PARA O SISTEMA CLIENTE
    if (argc != 3) {
        printf("Uso: %s <endereço IP> <porta>\n", argv[0]);
        return 1;
    }
    // RECEBE O ENDERECO E PORTA DO USUARIO NO TERMINAL
    char *enderecoIP = argv[1];
    int porta = atoi(argv[2]);
    
    // CRIACAO DO PACOTE E DEFINICAO DO DESCRITOR DE ARQUIVO DO CLIENTE
    int socketClienteDesc = criarSocketIpv4Tcp();
    
    // CRIA SE UMA NOVA ESTRUTURA DE TRANSPORTE IPV4, DADO IP E PORTA, E RETORNA UM PONTEIRO
    // COM SUA REPRESENTACAO BINARIA PARA A ESTRUTURA DE TRANSPORTE enderecoServidor.
    // VER--->>criarEnderecoIpv4 em libsocket.c
    struct sockaddr_in *enderecoServidor = criarEnderecoIpv4(enderecoIP, porta);

    // MAPEIA O PACOTE DO CLIENTE COM O ENDERECO DO SERVIDOR E REALIZA A CONEXAO
    connect(socketClienteDesc, enderecoServidor, sizeof *enderecoServidor);

    // INTERFACE --------------------------------------------------------------------
    char* nome = NULL;
    size_t tamanhoInicialNome = 0;
    printf("Qual o seu nome?: ");
    // DEFINICAO DE NOME PARA SEPARACAO DE USUARIOS NO CHAT
    ssize_t tamanhoNome = getline(&nome,&tamanhoInicialNome,stdin);
    nome[tamanhoNome-1] = 0;

    char* texto = NULL;
    size_t tamanhoInicialTexto = 0;
    printf("CLInteract 1.0 (CLiente) - (usuario: %s) (Sair do chat: !sair)\n", nome);
    // ------------------------------------------------------------------------------

    // RECEBIMENTO DE MENSAGENS
    threadBroadcastReceberMensagem(socketClienteDesc);

    // ENVIO DE MENSAGENS -----------------------------------------------------
    char mensagem[1024];

    while(true){
        
        ssize_t tamanhoTexto = getline(&texto,&tamanhoInicialTexto,stdin);
        texto[tamanhoTexto-1] = 0;        

        sprintf(mensagem,"%s: %s",nome,texto);

        if(tamanhoTexto>0)
        {
            if(strcmp(texto,"!sair")==0)
                break;
            send(socketClienteDesc, mensagem, strlen(mensagem), 0);
        }
    }
    //-------------------------------------------------------------------------
    close(socketClienteDesc);

    return 0;
}

void threadBroadcastReceberMensagem(int socketClienteDesc) {

    pthread_t id;
    pthread_create(&id, NULL, (void *(*)(void *)) receberMensagem, (void *) socketClienteDesc);

}

void receberMensagem(int socketClienteDesc) {
    char mensagem[1024];

    while(true)
    {
        ssize_t tamanhoMensagem = recv(socketClienteDesc, mensagem, 1024, 0);
        if(tamanhoMensagem > 0)
        {
            mensagem[tamanhoMensagem] = 0;
            printf("%s\n", mensagem);
        }
        if(tamanhoMensagem == 0)
            break;
    }

    close(socketClienteDesc);
}

