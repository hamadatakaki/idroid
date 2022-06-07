#pragma once

#include "../utils.h"

typedef struct sound_io_pointers {
    FILE *rec_fp;
    FILE *play_fp;
} SoundIO;

typedef struct client_io_pointers {
    SoundIO *sound;//popenの返したFILEポインタ
    int sfd;//サーバーのfd
} ClientIO;

typedef struct server_io_pointers {
    SoundIO *sound;//popenの返したFILEポインタ
    int clinet_socket_fd;//acceptの返り値
    int server_socket_fd;//サーバーのsocket
} ServerIO;

SoundIO *empty_sound_io();
ClientIO *empty_clinet_io();
ServerIO *empty_server_io();
void close_sound_io(SoundIO *);
void close_client_io(ClientIO *);
void close_server_io(ServerIO *);
