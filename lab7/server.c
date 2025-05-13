#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MAX_MSG_LEN 255
#define MAX_CLIENTS 10
#define SERVER_QUEUE_NAME "/server_queue"


typedef struct {
    int message_type; // init = 0 wiadomosc = 1
    int client_id;    // init = -1
    char queue_name[64];
    char mtext[MAX_MSG_LEN];
} client_message;

typedef struct {
    int message_type; // init = 0 wiadomosc = 1
    int client_id; 
    char mtext[MAX_MSG_LEN];
} server_message;

void handle_sigint(int sig) {  // usuwanie kolejki serwera po uzyciu ctrl+C 
    mq_unlink(SERVER_QUEUE_NAME);
    printf("\nKolejka serwera usunięta.\n");
    exit(0);
}

int main() {
    mqd_t client_queues[MAX_CLIENTS] = {0};
    int current_clients = 0;

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(client_message),
        .mq_curmsgs = 0
    };

    mqd_t server_mq = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDWR, 0660, &attr);
    if (server_mq == -1) {
        perror("mq_open (server)");
        exit(1);
    }
    signal(SIGINT, handle_sigint);

    printf("Serwer uruchomiony. Oczekiwanie na klientów...\n");

    while(1) {
        client_message msg;
        ssize_t bytes = mq_receive(server_mq, (char*)&msg, sizeof(msg), NULL);
        
        if (bytes == -1) {
            perror("mq_receive");
            continue;
        }

        if (msg.message_type == 0) { // INIT
            if (current_clients >= MAX_CLIENTS) {
                fprintf(stderr, "Maksymalna liczba klientów\n");
                continue;
            }

            mqd_t client_mq = mq_open(msg.queue_name, O_WRONLY);
            if (client_mq == -1) {
                perror("mq_open (kolejka klienta)");
                continue;
            }

            client_queues[current_clients] = client_mq;  // dodajemy klienta do tablicy

            server_message response = {  //wysylamy do klienta odpowiedz na init zawierającą jego id
                .message_type = 1,
                .client_id = current_clients  
            };

            if (mq_send(client_mq, (char*)&response, sizeof(response), 0) == -1) {
                perror("mq_send (odpowiedz na init)");
            } else {
                printf("Nowy klient: %s, ID: %d\n", msg.queue_name, current_clients);
                current_clients++;
            }

        } else if (msg.message_type == 1) { // zwykla wiadomosc
            if (msg.client_id < 0 || msg.client_id >= current_clients) {
                fprintf(stderr, "Nieprawidłowy ID klienta: %d\n", msg.client_id);
                continue;
            }

            server_message response = {
                .message_type = 1,
                .client_id = msg.client_id
            };
            strncpy(response.mtext, msg.mtext, MAX_MSG_LEN - 1);
            response.mtext[MAX_MSG_LEN - 1] = '\0';

            for (int i = 0; i < current_clients; i++) {
                if (i == msg.client_id) continue;
                if (mq_send(client_queues[i], (char*)&response, sizeof(response), 0) == -1) {
                    perror("mq_send (wiadomosci)");
                }
            }
        }
    }
    return 0;
}