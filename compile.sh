#!/usr/bin/env bash

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit
fi
gcc -fPIE -static-pie -Wall -Wl,-z,relro,-z,now -owordle.so ./wordle.c -fstack-protector-all
strip ./wordle.so
checksec ./wordle.so
chown root:root ./wordle.so
chmod 4755 ./wordle.so
