#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define INITIALIZE(class) ((class *) malloc(sizeof(class)))
#define MAX(u, v) ((u > v) ? u : v)
#define MIN(u, v) ((u > v) ? v : u)
