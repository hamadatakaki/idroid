#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INITIALIZE(class) ((class *)malloc(sizeof(class)))
#define MAX(u, v) ((u > v) ? u : v)
#define MIN(u, v) ((u > v) ? v : u)
