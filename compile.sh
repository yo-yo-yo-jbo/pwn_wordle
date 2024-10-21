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
printf NEVER_TELL_THE_TRUTH_TO_PEOPLE_WHO_ARE_NOT_WORTHY_OF_IT > ./flag.txt
chmod 600 ./flag.txt
chown root:root ./dictionary.txt

