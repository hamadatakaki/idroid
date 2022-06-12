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
    for (int i = 0; i < CLIENT_MAX_NUM; i++) {
        sio->client_socket_fd[i] = -1;
    }
    sio->max_client_fd = -1;
    sio->client_num = 0;
    sio->server_socket_fd = -1;
    sio->sound = empty_sound_io();

    return sio;
}

void close_server_io(ServerIO *sio) {
    for (int i = 0; i < sio->client_num; i++) {
        if (sio->client_socket_fd >= 0) {
            close(sio->client_socket_fd[i]);
        }
    }
    if (sio->server_socket_fd >= 0) {
        close(sio->server_socket_fd);
    }
    close_sound_io(sio->sound);
    free(sio);
}

int add_client(ServerIO *sio, int client_fd) {
    if (sio->client_num >= CLIENT_MAX_NUM) {
        return -1;
    }
    sio->client_socket_fd[sio->client_num] = client_fd;
    if (client_fd > sio->max_client_fd) {
        sio->max_client_fd = client_fd;
    }
    sio->client_num += 1;
    return sio->client_num;
}