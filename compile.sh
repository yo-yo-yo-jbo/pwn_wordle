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
printf INTENT{WORDS_CAN_CARRY_ANY_BURDEN_WE_WISH_ALL_THATS_REQUIRED_IS_AGREEMENT_AND_A_TRADITION_UPON_WHICH_TO_BUILD} > ./flag.txt
chmod 600 ./flag.txt
chown root:root ./dictionary.txt

