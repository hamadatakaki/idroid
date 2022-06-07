#include "io/pointers.h"
#include "io/launch.h"

#include <pthread.h>

#define N 512

 typedef struct share_data{
    ServerIO *sio;
    // char send_data[N];
    // char recv_data[N];
} ShareData;

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <port>: launch server\n");
    printf("  $cmd help:   show usage\n");
}
void play(void *arg){
    ShareData *pd = (ShareData *)arg;
    ServerIO *sio = pd->sio;
    char recv_data[N];
    int n;
    while(1){
        // play: receive and write
        n = recv(sio->clinet_socket_fd, recv_data, N, 0);
        
        if (n < 0) {
            close_server_io(sio);
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        n = fwrite(recv_data, sizeof(char), n, sio->sound->play_fp);
    }

}

void rec(void *arg){
    ShareData *pd = (ShareData *)arg;
    ServerIO *sio = pd->sio;
    char send_data[N];
    int n;
    // rec: read and send
    while(1){
        n = fread(send_data, sizeof(char), N, sio->sound->rec_fp);
        
        if (n < 0) {
            close_server_io(sio);
            fprintf(stderr, "[error] read invalid string\n");
            exit(1);
        }

        send(sio->clinet_socket_fd, send_data, n, 0);
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

    // preprocess - open sound IO
    int err = open_stdio(sio->sound);
    if (err) {
        close_server_io(sio);
        return 1;
    }

    ShareData share_data[2];
    pthread_t t[2];
    share_data[0].sio = sio;
    share_data[1].sio = sio;
    pthread_create(&t[0],NULL,(void*)rec,&share_data[0]);
    pthread_create(&t[1],NULL,(void*)play,&share_data[1]);
    pthread_join(t[0],NULL);
    pthread_join(t[1],NULL);

    return 0;
}