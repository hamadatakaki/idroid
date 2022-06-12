#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "pointers.h"

// Server
void launch_server(int port, ServerIO *sio);
int accept_new_client(ServerIO *sio);

// Client
void launch_client(char *ip, int port, ClientIO *cio);

// Sound IO
int open_stdio(SoundIO *sound);
