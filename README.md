# Aplicação Interact (Linux)
Implementação de Sockets em C para apresentação de trabalho em Sistemas Distribuidos

## Servidor (Console)
- para compilar o Servidor, abra o terminal, vá ao diretório da pasta Servidor e execute o seguinte comando:
- gcc main.c libsocket.c -o Interact-server -pthread -w
- execute o servidor em um terminal, com o comando ./Interact-server
  
- O servidor é executado em localhost e IP alocado a máquina em execução do serviço.
- O cliente deve rodar na mesma rede que o servidor para funcionar corretamente.

## Interact 1.0 (Console)
- para compilar o Cliente, abra o terminal, vá ao diretório da pasta Interact-1.0 e execute o seguinte comando:
- gcc main.c libsocket.c -o Interact-cli -pthread -w
- execute o cliente em um terminal, com o comando ./Interact-cli

## Interact 2.0 (GTK 3.0)
- para compilar o Cliente, abra o terminal, vá ao diretório da pasta Interact-2.0 e execute o seguinte comando:
- gcc main.c libsocket.c -o o Interact-gtk -pthread $(pkg-config --cflags --libs gtk+-3.0) -w
- clique duas vezes no aplicativo gerado: Interact-gtk, para executar a aplicação.







