#include "pointers.h"

SoundIO *empty_sound_io() {
    SoundIO *sound = INITIALIZE(SoundIO);
    sound->play_fp = NULL;
    sound->rec_fp = NULL;
    return sound;
}

void close_sound_io(SoundIO *sound) {
    if (sound->rec_fp != NULL) {
        pclose(sound->rec_fp);
    }
    if (sound->play_fp != NULL) {
        pclose(sound->play_fp);
    }
    free(sound);
}

ClientIO *empty_clinet_io() {
    ClientIO *cio = INITIALIZE(ClientIO);

    cio->sfd = -1;
    cio->sound = empty_sound_io();

    return cio;
}

void close_client_io(ClientIO *cio) {
    if (cio->sfd >= 0) {
        close(cio->sfd);
    }
    close_sound_io(cio->sound);
    free(cio);
}

ServerIO *empty_server_io() {
    ServerIO *sio = INITIALIZE(ServerIO);
    // server socket
    sio->server_socket_fd = -1;

    // client sockets
    for (int i = 0; i < MAX_CLIENT_SIZE; i++) {
        sio->client_socket_fd[i] = -1;
    }
    sio->max_client_fd = -1;
    sio->client_size = 0;

    return sio;
}

void close_server_io(ServerIO *sio) {
    for (int i = 0; i < sio->client_size; i++) {
        if (sio->client_socket_fd >= 0) {
            close(sio->client_socket_fd[i]);
        }
    }
    if (sio->server_socket_fd >= 0) {
        close(sio->server_socket_fd);
    }
    free(sio);
}

int add_client(ServerIO *sio, int client_fd) {
    // ServerIO が現在のクライアント数なども保存するため、更新処理を同時に行う
    if (sio->client_size >= MAX_CLIENT_SIZE) {
        return -1;
    }

    sio->client_socket_fd[sio->client_size] = client_fd;
    if (client_fd > sio->max_client_fd) {
        sio->max_client_fd = client_fd;
    }
    sio->client_size += 1;
    return sio->client_size;
}
