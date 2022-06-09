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

void recv_data(void *arg){
    ShareData *pd = (ShareData *)arg;
    ServerIO *sio = pd->sio;
    char recv_data[N];
    int n;
    while(1){

    }
}



/*void play(void *arg){
    ShareData *pd = (ShareData *)arg;
    ServerIO *sio = pd->sio;
    char recv_data[N];
    int n;
    while(1){
        // play: receive and write
        n = recv(sio->client_socket_fd, recv_data, N, 0);
        
        if (n < 0) {
            close_server_io(sio);
            fprintf(stderr, "[error] receive invalid string\n");
            exit(1);
        }

        n = fwrite(recv_data, sizeof(char), n, sio->sound->play_fp);
    }

}*/

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
        for (int i = 0; i < sio->client_num; i++){
            send(sio->client_socket_fd[i], send_data, n, 0);
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
    ServerIO *sio;
    sio = empty_server_io();
    
    ShareData share_data;
    share_data.sio = sio;
    pthread_t t[CLIENT_NUM*2];
    char buf[2048];
    fd_set fds, readfds;
    FD_ZERO(&readfds);
    while(1){
        // preprocess - launch server
        launch_server(port, sio);// sioへの書き込み
        printf("launch server %d\n",sio->client_num);
        // preprocess - open sound IO
        
        int err = open_stdio(sio->sound);
        if (err) {
            printf("error\n");
            close_server_io(sio);
            return 1;
        }
        FD_SET(sio->client_socket_fd[sio->client_num],&readfds);
        memcpy(&fds, &readfds, sizeof(fd_set));
        select(sio->max_client_fd, &fds, NULL,NULL,NULL);
        printf("cccc");
        for(int i = 0; i < sio->client_num; i++){
            printf("aaaa");
            if (FD_ISSET(sio->client_socket_fd[sio->client_num],&fds)){
                memset(buf, 0, sizeof(buf));
                printf("bbbb");
                int n = recv(sio->client_socket_fd[sio->client_num], buf, sizeof(buf), 0);
                printf("n is %d\n",n);
                n = fwrite(buf, sizeof(char), n, sio->sound->play_fp);
            }
        }
    }

        // share_data[2*i+1].sio = sio;
        pthread_create(&t[0],NULL,(void*)rec,&share_data);
        // pthread_create(&t[2*i+1],NULL,(void*)play,&share_data[2*i+1]);
        pthread_join(t[0],NULL);
        // pthread_join(t[2*i+1],NULL);

    
    

    

    

    return 0;
}