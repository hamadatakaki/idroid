#include "io/pointers.h"
#include "io/launch.h"

#include <pthread.h>

#define N 512
#define CLIENT_NUM 10// number of client

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
    ServerIO *list_sio[CLIENT_NUM];
    for(int i = 0;i<CLIENT_NUM;i++){
        list_sio[i] = empty_server_io();
    }

    ShareData share_data[CLIENT_NUM*2];
    pthread_t t[CLIENT_NUM*2];

    // preprocess - launch server
    for (int i = 0; i < CLIENT_NUM; i++){
        printf("launch server %d\n",i);
        launch_server(port, list_sio[i]);// sioへの書き込み
        printf("connect success\n");
        // preprocess - open sound IO
        int err = open_stdio(list_sio[i]->sound);
        if (err) {
            close_server_io(list_sio[i]);
            return 1;
        }
        
        share_data[2*i].sio = list_sio[i];
        share_data[2*i+1].sio = list_sio[i];
        pthread_create(&t[2*i],NULL,(void*)rec,&share_data[2*i]);
        pthread_create(&t[2*i+1],NULL,(void*)play,&share_data[2*i+1]);
        pthread_join(t[2*i],NULL);
        pthread_join(t[2*i+1],NULL);

    }
    

    

    

    return 0;
}