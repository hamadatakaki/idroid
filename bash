#!/bin/bash
rec -t raw -b 16 -c 1 -e s -r 44100 - | ./iphone 192.168.100.7 60000 | play -t raw -b 16 -c 1 -e s -r 44100 -