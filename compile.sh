#!/usr/bin/env bash

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit
fi
FILE_NAME=${FILE_NAME:-"wordle.elf"}
gcc -fPIE -static-pie -Wall -Wl,-z,relro,-z,now -o"${FILE_NAME}" ./wordle.c -fstack-protector-all
strip "./${FILE_NAME}"
checksec "./${FILE_NAME}"
chown root:root "./${FILE_NAME}"
chmod 4755 "./${FILE_NAME}"
