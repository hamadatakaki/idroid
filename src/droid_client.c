#include "io/pointers.h"
#include "io/launch.h"

#include <pthread.h>

#define N 512

typedef struct client_share_data{
    ClientIO *cio;
    // char send_data[N];
    // char recv_data[N];
} ClientShareData;

void show_usage() {
    printf("usage:\n");
    printf("  $cmd <server-ip> <port>: launch server\n");
    // printf("  $cmd help:   show usage\n");
}

void play(ClientIO *cio) {
    int n;
    char data[N];

    while(1){
        // play: receive and write
        n = recv(cio->sfd, data, N, 0);
        
        if (n < 0) {
            close_client_io(cio);
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        n = fwrite(data, sizeof(char), n, cio->sound->play_fp);
    } 
}

void rec(ClientIO *cio) {
    int n;
    char data[N];

    while(1){
        // rec: read and send
        n = fread(data, sizeof(char), N, cio->sound->rec_fp);
        
        if (n < 0) {
            close_client_io(cio);
            fprintf(stderr, "[error] read invalid string\n");
            exit(1);
        }

        send(cio->sfd, data, n, 0);
    }
}

void play_on_pthread(void *arg){
    ClientShareData *pd = (ClientShareData *)arg;
    play(pd->cio);
}

void rec_on_pthread(void *arg){
    ClientShareData *pd = (ClientShareData *)arg;
    rec(pd->cio);
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

    ClientShareData *share = INITIALIZE(ClientShareData);
    share->cio = cio;
    
    pthread_t t_rec, t_play;
    pthread_create(&t_rec, NULL,(void*)rec, share);
    pthread_create(&t_play, NULL,(void*)play, share);
    pthread_join(t_rec,NULL);
    pthread_join(t_play, NULL);

    return 0;
}