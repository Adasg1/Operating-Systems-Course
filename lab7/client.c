#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_MSG_LEN 255
#define SERVER_QUEUE_NAME "/server_queue"
char queue_name[64];

typedef struct {
    int message_type; // init = 0 wiadomosc = 1
    int client_id; // init = -1, pozostale to id klientow
    char queue_name[64];
    char mtext[MAX_MSG_LEN];
} client_message;

typedef struct {
    int message_type; // init = 0, wiadomosc = 1
    int client_id;
    char mtext[MAX_MSG_LEN];
} server_message;

void handle_sigint(int sig) {  // usuwanie kolejki klienta przy uzyciu ctrl+C 
    mq_unlink(queue_name);
    printf("\nKolejka klienta usunięta.\n");
    exit(0);
}

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(server_message);
    attr.mq_curmsgs = 0;

    snprintf(queue_name, sizeof(queue_name), "/client_%d", getpid());  // unikalna nazwa kolejki dla klienta

    mqd_t client_mq = mq_open(queue_name, O_CREAT | O_RDONLY, 0666, &attr);

    mqd_t server_mq = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    

    client_message msg = {
        .message_type = 0,
        .client_id = -1,
        .queue_name = {0},
        .mtext = {0}
    };

    strncpy(msg.queue_name, queue_name, sizeof(msg.queue_name) - 1);
    msg.queue_name[sizeof(msg.queue_name) - 1] = '\0';

    mq_send(server_mq, (const char*)&msg, sizeof(msg), 0); //wysylamy init do serwera

    server_message received_msg;

    ssize_t success = mq_receive(client_mq, (char*)&received_msg, sizeof(received_msg), NULL);
    if (success == -1) perror("receive (klient)");

    msg.client_id = received_msg.client_id;
    msg.message_type = 1;

    if (fork() == 0) {  // proces dziecka odbiera wiadomosci od serwera i wypisuje w konsoli
        while(1) {
            int success = mq_receive(client_mq, (char*)&received_msg, sizeof(received_msg), NULL);
            if (received_msg.message_type == 1) printf("%d: %s", received_msg.client_id, received_msg.mtext);
            if (success == -1) perror(NULL);
        }
    } else {           // rodzic czyta z konsoli i wysyla wiadomosc do serwera
        while(1) {
            signal(SIGINT, handle_sigint);
            fgets(msg.mtext, MAX_MSG_LEN, stdin);
            int success = mq_send(server_mq, (char*)&msg, sizeof(msg), 0);
            if (success == -1) perror(NULL);
        }
    }
}