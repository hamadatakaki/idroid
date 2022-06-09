#include "io/pointers.h"
#include "io/launch.h"

#include <pthread.h>

#define N 512

 typedef struct share_data{
    ClientIO *cio;
    // char send_data[N];
    // char recv_data[N];
} ShareData;

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <server-ip> <port>: launch server\n");
    // printf("  $cmd help:   show usage\n");
}

void play(void *arg){
    ShareData *pd = (ShareData *)arg;
    ClientIO *cio = pd->cio;
    char recv_data[N];
    int n;
    while(1){
        // play: receive and write
        n = recv(cio->sfd, recv_data, N, 0);
        
        if (n < 0) {
            close_client_io(cio);
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        n = fwrite(recv_data, sizeof(char), n, cio->sound->play_fp);
    }

}

void rec(void *arg){
    ShareData *pd = (ShareData *)arg;
    ClientIO *cio = pd->cio;
    char send_data[N];
    int n;
    // rec: read and send
    while(1){
        n = fread(send_data, sizeof(char), N, cio->sound->rec_fp);
        
        if (n < 0) {
            close_client_io(cio);
            fprintf(stderr, "[error] read invalid string\n");
            exit(1);
        }

        send(cio->sfd, send_data, n, 0);
    }
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
    launch_client(ip_addr, port, cio);//cioに書き込み

    // preprocess - open sound IO
    int err = open_stdio(cio->sound);
    if (err) {
        close_client_io(cio);
        return 1;
    }

    ShareData share_data[2];
    pthread_t t[2];
    share_data[0].cio = cio;
    share_data[1].cio = cio;
    pthread_create(&t[0],NULL,(void*)rec,&share_data[0]);
    pthread_create(&t[1],NULL,(void*)play,&share_data[1]);
    pthread_join(t[0],NULL);
    pthread_join(t[1],NULL);

    return 0;
}