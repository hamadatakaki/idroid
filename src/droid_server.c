#include "io/launch.h"
#include "io/pointers.h"

#include <pthread.h>

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <port>: launch server\n");
    printf("  $cmd help:   show usage\n");
}

void through(ServerIO *sio, int client_from_fd) {
    int n;
    char data[DATA_FETCH_SIZE];

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

            if (client_from_fd == client_to_fd) {
                break;
            } else {
                send(client_to_fd, data, n, 0);
            }
        }
    }
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

    // accept, pth_create, pth_joinのくりかえし

    char buf[2048];
    fd_set fds, readfds;
    FD_ZERO(&readfds);
    while (1) {
        //
        FD_SET(sio->client_socket_fd[sio->client_size], &readfds);
        memcpy(&fds, &readfds, sizeof(fd_set));
        select(sio->max_client_fd, &fds, NULL, NULL, NULL);
        printf("cccc");
        for (int i = 0; i < sio->client_size; i++) {
            printf("aaaa");
            if (FD_ISSET(sio->client_socket_fd[sio->client_size], &fds)) {
                memset(buf, 0, sizeof(buf));
                printf("bbbb");
                int n = recv(sio->client_socket_fd[sio->client_size], buf, sizeof(buf), 0);
                printf("n is %d\n", n);
            }
        }
    }

    // share_data[2*i+1].sio = sio;
    // pthread_create(&t[0], NULL, (void *)rec, &share_data);
    // pthread_create(&t[2*i+1],NULL,(void*)play,&share_data[2*i+1]);
    // pthread_join(t[0], NULL);
    // pthread_join(t[2*i+1],NULL);

    return 0;
}