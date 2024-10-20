#!/bin/bash
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit
fi
gcc -fPIE -static-pie -Wall -Wl,-z,relro,-z,now -owordle ./wordle.c -fstack-protector-all
strip ./wordle
checksec ./wordle
chown root:root ./wordle
chmod 4755 ./wordle
echo this_is_out_flag > ./flag.txt
chmod 600 ./flag.txt
