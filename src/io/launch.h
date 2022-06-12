#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "pointers.h"

void launch_server(int port, ServerIO *sio);
void launch_client(char *ip, int port, ClientIO *cio);
int open_stdio(SoundIO *sound);
