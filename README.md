# clinteract
Implementação de Sockets em C para apresentação de trabalho em Sistemas Distribuidos

para compilar o Cliente, abra o terminal, vá ao diretório da pasta Cliente e execute o seguinte comando:

gcc main.c libsocket.c -o CLInteract-cliente -pthread -w

para compilar o Servidor, abra o terminal, vá ao diretório da pasta Servidor e execute o seguinte comando:

gcc main.c libsocket.c -o CLInteract-servidor -pthread -w

O servidor é executado em localhost e IP alocado a máquina em execução do serviço.
