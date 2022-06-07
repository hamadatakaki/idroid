#include "io/pointers.h"
#include "io/launch.h"

#define N 512

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <server-ip> <port>: launch server\n");
    // printf("  $cmd help:   show usage\n");
}

int main(int argc, char **argv) {
    // preprocess - parse arguments.
    if (argc != 3) {
        fprintf(stderr, "[error] invalid command line arguments\n");
        show_usage();
        return 1;
    }

    char *ip_addr = argv[1];
    int port = atoi(argv[2]);
    ClientIO *cio = empty_clinet_io();

    // preprocess - launch client
    printf("launch client\n");
    launch_client(ip_addr, port, cio);

    // preprocess - open sound IO
    int err = open_stdio(cio->sound);
    if (err) {
        close_client_io(cio);
        return 1;
    }

    int n;
    char data[N];

    while(1) {
        // play: receive and write
        n = recv(cio->sfd, data, N, 0);
        
        if (n < 0) {
            close_client_io(cio);
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        n = fwrite(data, sizeof(char), n, cio->sound->play_fp);

        // rec: read and send
        n = fread(data, sizeof(char), N, cio->sound->rec_fp);
        
        if (n < 0) {
            close_client_io(cio);
            fprintf(stderr, "[error] read invalid string\n");
            exit(1);
        }

        send(cio->sfd, data, n, 0);
    }

    return 0;
}