#include "io/launch.h"
#include "io/pointers.h"

#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct server_share_data {
    ServerIO *sio;
    int client_from_fd;
} ServerShareData;

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <port>: launch server\n");
    printf("  $cmd help:   show usage\n");
}

void through(ServerIO *sio, int client_from_fd) {
    int n;
    char data[DATA_FETCH_SIZE];

    // fd_set fds, readfds;

    while (1) {
        n = recv(client_from_fd, data, DATA_FETCH_SIZE, 0);

        if (n < 0) {
            // close_client_io(cio);  // TODO: 適切なclose
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        // send
        for (int i = 0; i < sio->client_size; i++) {
            int client_to_fd = sio->client_socket_fd[i];

            if (client_from_fd != client_to_fd) {
                send(client_to_fd, data, n / sio->client_size, 0);
            } else {
                continue;
            }
        }
    }
}

void through_on_pthread(void *arg) {
    ServerShareData *share = (ServerShareData *)arg;
    through(share->sio, share->client_from_fd);
}

int main(int argc, char **argv) {
    // preprocess - parse arguments.
    if (argc != 2) {
        fprintf(stderr, "[error] invalid command line arguments\n");
        show_usage();
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        show_usage();
        return 0;
    }

    int port = atoi(argv[1]);
    ServerIO *sio = empty_server_io();

    // preprocess - launch server
    printf("launch server\n");
    launch_server(port, sio);

    // preprocess - thread initialize
    pthread_t threads[MAX_CLIENT_SIZE];
    ServerShareData *shares[MAX_CLIENT_SIZE];

    for (int i = 0; i < MAX_CLIENT_SIZE; i++) {
        shares[i] = INITIALIZE(ServerShareData);
        shares[i]->sio = sio;
    }

    // main routine
    for (int i = 0; i < MAX_CLIENT_SIZE + 1; i++) {
        // clientの同時接続数が可能な範囲で最大に到達
        if (i >= MAX_CLIENT_SIZE) {
            fprintf(stdout, "[warning] max clients accessed.\n");
            break;
        }

        printf("[log] waiting access from client%02d.\n", i);

        // accept, pth_create, pth_join
        shares[i]->client_from_fd = accept_new_client(sio);
        pthread_create(&threads[i], NULL, (void *)through_on_pthread, shares[i]);
        pthread_detach(threads[i]);
        fprintf(stdout, "[log] client%02d connected\n", i);
        fprintf(stdout, "[log]     fd: %d\n", shares[i]->client_from_fd);
    }

    return 0;
}