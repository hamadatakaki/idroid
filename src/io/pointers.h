#pragma once

#include "../utils.h"

typedef struct sound_io_pointers {
    FILE *rec_fp;
    FILE *play_fp;
} SoundIO;

typedef struct client_io_pointers {
    SoundIO *sound;
    int sfd;
} ClientIO;

typedef struct server_io_pointers {
    SoundIO *sound;
    int clinet_socket_fd;
    int server_socket_fd;
} ServerIO;

SoundIO *empty_sound_io();
ClientIO *empty_clinet_io();
ServerIO *empty_server_io();
void close_sound_io(SoundIO *);
void close_client_io(ClientIO *);
void close_server_io(ServerIO *);
