#include "launch.h"

#include <assert.h>

void launch_server(int port, ServerIO *sio) {
    // server socket fd
    int ssfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in *addr = INITIALIZE(struct sockaddr_in);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
    bind(ssfd, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    listen(ssfd, 10);

    sio->server_socket_fd = ssfd;
}

int accept_new_client(ServerIO *sio) {
    assert(sio->server_socket_fd >= 0);

    struct sockaddr_in *client_addr = INITIALIZE(struct sockaddr_in);
    socklen_t _len;
    int csfd = accept(sio->server_socket_fd, (struct sockaddr *)client_addr, &_len);
    int result = add_client(sio, csfd);

    assert(result >= 0);
    return result;
}

void launch_client(char *ip, int port, ClientIO *cio) {
    // client socket fd
    int fd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in *addr = INITIALIZE(struct sockaddr_in);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);
    connect(fd, (struct sockaddr *)addr, sizeof(struct sockaddr_in));

    cio->sfd = fd;
}

int open_stdio(SoundIO *sound) {
    FILE *rec_fp = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r");
    FILE *play_fp = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w");

    int rec_ng = rec_fp == NULL;
    int play_ng = play_fp == NULL;

    if (rec_ng || play_ng) {
        if (rec_ng)
            fprintf(stderr, "[error] failed exec `rec`.\n");
        if (play_ng)
            fprintf(stderr, "[error] failed exec `play`.\n");
        return 1;
    }

    sound->rec_fp = rec_fp;
    sound->play_fp = play_fp;

    return 0;
}