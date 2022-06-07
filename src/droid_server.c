#include "io/pointers.h"
#include "io/launch.h"

#define N 512

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <port>: launch server\n");
    printf("  $cmd help:   show usage\n");
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

    // preprocess - open sound IO
    int err = open_stdio(sio->sound);
    if (err) {
        close_server_io(sio);
        return 1;
    }

    int n;
    char data[N];

    while(1) {
        // rec: read and send
        n = fread(data, sizeof(char), N, sio->sound->rec_fp);
        
        if (n < 0) {
            close_server_io(sio);
            fprintf(stderr, "[error] read invalid string\n");
            exit(1);
        }

        send(sio->clinet_socket_fd, data, n, 0);

        // play: receive and write
        n = recv(sio->clinet_socket_fd, data, N, 0);
        
        if (n < 0) {
            close_server_io(sio);
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        n = fwrite(data, sizeof(char), n, sio->sound->play_fp);
    }

    return 0;
}