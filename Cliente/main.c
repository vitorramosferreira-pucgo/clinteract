
//Interact-cliente - Vitor Ramos Ferreira

#include <gtk/gtk.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "libsocket.h"

#define MAX_MESSAGE_LEN 1024

GtkWidget *chat_view;
GtkWidget *entry;
GtkWidget *scrolled_window;
char *nome;
int socketClienteDesc;

void enviarMensagem(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    char mensagem[MAX_MESSAGE_LEN];

    if (strlen(text) > 0) {
        sprintf(mensagem, "%s: %s\n", nome, text);
        send(socketClienteDesc, mensagem, strlen(mensagem), 0);

        gtk_entry_set_text(GTK_ENTRY(entry), "");

        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
        gtk_text_buffer_insert_at_cursor(buffer, mensagem, -1);
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(chat_view), &iter, 0.0, FALSE, 0.0, 1.0);
    }
}

void receberMensagem() {
    char mensagem[MAX_MESSAGE_LEN];

    while (true) {
        ssize_t tamanhoMensagem = recv(socketClienteDesc, mensagem, MAX_MESSAGE_LEN, 0);
        if (tamanhoMensagem > 0) {
            mensagem[tamanhoMensagem] = 0;

            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
            GtkTextIter iter;
            gtk_text_buffer_get_end_iter(buffer, &iter);
            gtk_text_buffer_insert(buffer, &iter, mensagem, -1);
            gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(chat_view), &iter, 0.0, FALSE, 0.0, 1.0);
        }
        if (tamanhoMensagem == 0)
            break;
    }

    close(socketClienteDesc);
}

void threadBroadcastReceberMensagem() {
    pthread_t id;
    pthread_create(&id, NULL, (void *(*)(void *))receberMensagem, NULL);
}

void fecharJanela(GtkWidget *widget, gpointer data) {
    close(socketClienteDesc);
    gtk_main_quit();
}

void obterNome() {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *entry_dialog;
    gint resposta;

    dialog = gtk_dialog_new_with_buttons("Interact 2.0 - Usuário",
                                         NULL,
                                         GTK_DIALOG_MODAL,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    entry_dialog = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_dialog), "Digite seu nome");

    gtk_container_add(GTK_CONTAINER(content_area), entry_dialog);

    gtk_widget_show_all(dialog);

    resposta = gtk_dialog_run(GTK_DIALOG(dialog));

    if (resposta == GTK_RESPONSE_OK) {
        nome = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_dialog)));
    } else {
        g_print("Operação cancelada\n");
        exit(1);
    }

    gtk_widget_destroy(dialog);
}

void obterEnderecoEServidor() {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *ip_entry;
    GtkWidget *port_entry;
    gint resposta;

    dialog = gtk_dialog_new_with_buttons("Interact 2.0 - Servidor",
                                         NULL,
                                         GTK_DIALOG_MODAL,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         NULL);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 220, 100);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    ip_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ip_entry), "Endereço IP do Servidor");

    port_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(port_entry), "Número da Porta do Servidor");

    gtk_container_add(GTK_CONTAINER(content_area), ip_entry);
    gtk_container_add(GTK_CONTAINER(content_area), port_entry);

    gtk_widget_show_all(dialog);

    resposta = gtk_dialog_run(GTK_DIALOG(dialog));

    if (resposta == GTK_RESPONSE_OK) {
        char *enderecoIP = g_strdup(gtk_entry_get_text(GTK_ENTRY(ip_entry)));
        int porta = atoi(gtk_entry_get_text(GTK_ENTRY(port_entry)));

        socketClienteDesc = criarSocketIpv4Tcp();
        struct sockaddr_in *enderecoServidor = criarEnderecoIpv4(enderecoIP, porta);
        connect(socketClienteDesc, enderecoServidor, sizeof *enderecoServidor);

        g_free(enderecoIP);
    } else {
        g_print("Operação cancelada\n");
        exit(1);
    }

    gtk_widget_destroy(dialog);
}

void mainSendButtonActivate(GtkWidget *widget, gpointer data) {
    enviarMensagem(widget, data);
}

int main(int argc, char *argv[]) {

    //INICIA UMA INSTÂNCIA GTK
    gtk_init(&argc, &argv);

    //OBTEM O ENDEREÇO IP E PORTA DO SERVIDOR DO USUÁRIO
    obterEnderecoEServidor();

    //OBTEM A ENTRADA DO NOME DO USUÁRIO
    obterNome();

    //CRIA UMA JANELA GTK COM O TÍTULO "Interact 2.0 (Cliente)"
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Interact 2.0 (Cliente)");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(fecharJanela), NULL);

    //POSICIONA A JANELA PRINCIPAL NO CENTRO DA TELA
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    //CRIA UMA JANELA COM O RECURSO DE ROLAGEM
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    //CRIA UMA ÁREA DE TEXTO PARA EXIBIR AS MENSAGENS DENTRO DA AREA DE ROLAGEM
    chat_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), GTK_WRAP_WORD_CHAR);

    //CRIA UMA CAIXA DE TEXTO PARA ENTRADA DE MENSAGENS
    entry = gtk_entry_new();
    GtkWidget *send_button = gtk_button_new_with_label("Enviar");

    //CONECTA OS SINAIS "activate" E "clicked" DA CAIXA DE TEXTO E DO BOTÃO DE ENVIAR
    //E ATIVA A FUNÇÃO mainSendButtonActivate QUANDO O USUÁRIO PRESSIONA A TECLA ENTER OU O BOTÃO DE ENVIAR
    //NO MOMENTO DE ENVIO DA MENSAGEM
    g_signal_connect(entry, "activate", G_CALLBACK(mainSendButtonActivate), NULL);
    g_signal_connect(send_button, "clicked", G_CALLBACK(mainSendButtonActivate), NULL);

    //CRIA UM CONTAINER PARA ACOMODAR A ÁREA DE TEXTO, A JANELA DE ROLAGEM E A CAIXA DE ENTRADA DE MENSAGENS
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    //ADICIONA OS WIDGETS AO CONTAINER
    gtk_container_add(GTK_CONTAINER(scrolled_window), chat_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    //MOSTRA TODOS OS WIDGETS
    gtk_widget_show_all(window);

    //INICIA A THREAD PARA RECEBER AS MENSAGENS DO SERVIDOR
    threadBroadcastReceberMensagem();

    //INICIA O LOOP PRINCIPAL DA APLICAÇÃO
    gtk_main();

    return 0;
}
