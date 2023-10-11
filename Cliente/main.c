//CLInteract-cliente - Vitor Ramos Ferreira

//para compilar: gcc main.c libsocket.c -o CLInteract-cliente -pthread -w

#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "libsocket.h"

void threadBroadcastReceberMensagem(int socketClienteDesc);

void receberMensagem(int socketClienteDesc);

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Uso: %s <endereço IP> <porta>\n", argv[0]);
        return 1;
    }

    char *enderecoIP = argv[1];
    int porta = atoi(argv[2]);

    int socketClienteDesc = criarSocketIpv4Tcp();

    struct sockaddr_in *enderecoServidor = criarEnderecoIpv4(enderecoIP, porta);

    connect(socketClienteDesc, enderecoServidor, sizeof *enderecoServidor);

    char* nome = NULL;
    size_t tamanhoInicialNome = 0;
    printf("Qual o seu nome?: ");
    ssize_t tamanhoNome = getline(&nome,&tamanhoInicialNome,stdin);
    nome[tamanhoNome-1] = 0;

    char* texto = NULL;
    size_t tamanhoInicialTexto = 0;
    printf("CLInteract 1.0 (CLiente) - (usuario: %s) (Sair do chat: !sair)\n", nome);

    threadBroadcastReceberMensagem(socketClienteDesc);

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
            printf("Resposta do cliente: %s\n", mensagem);
        }
        if(tamanhoMensagem == 0)
            break;
    }

    close(socketClienteDesc);
}

