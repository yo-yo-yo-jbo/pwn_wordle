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
printf YOU_ARE_A_BEING_UNTO_YOURSELF_YOU_ARE_A_LAW_UNTO_YOURSELF_EACH_INTELLIGENCE_IS_HOLY_FOR_ALL_THAT_LIVES_IS_HOLY > ./flag.txt
chmod 600 ./flag.txt
