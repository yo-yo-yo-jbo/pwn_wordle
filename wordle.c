#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LENGTH (6)
#define MAX_WORDS (10000)
#define MAX_DICTIONARY_SIZE (WORD_LENGTH * MAX_WORDS)
#define ATTEMPTS_NUM (5)

/************************************************************************
*                                                                       *
*  Function:   write_error                                              *
*  Purpose:    Prints out an error message.                             *
*  Parameters: - msg - the message to print out.                        *
*                                                                       *
*************************************************************************/
static
void
write_error(
    char* msg
)
{
    // Write in color
    (void)printf("\x1b[31m\x1b[1mERROR:\x1b[0m %s\n", msg);
}

/************************************************************************
*                                                                       *
*  Function:   print_logo                                               *
*  Purpose:    Clears the screen and prints out the logo.               *
*                                                                       *
*************************************************************************/
static
void
print_logo(void)
{
    (void)printf("\e[1;1H\e[2J");
    (void)printf("\n\x1b[37m\x1b[1m  ██     ██   \x1b[31m\x1b[1m,d88b.d88b,\x1b[37m\x1b[1m  ██████  ██████  ██      ███████\n  ██     ██   \x1b[31m\x1b[1m88888888888\x1b[37m\x1b[1m  ██   ██ ██   ██ ██      ██     \n  ██  █  ██   \x1b[31m\x1b[1m`Y8888888Y\'\x1b[37m\x1b[1m  ██████  ██   ██ ██      █████  \n  ██ ███ ██     \x1b[31m\x1b[1m`Y888Y\'\x1b[37m\x1b[1m    ██   ██ ██   ██ ██      ██     \n   ███ ███        \x1b[31m\x1b[1m`Y\'\x1b[37m\x1b[1m      ██   ██ ██████  ███████ ███████\n\n             Level by Jonathan Bar Or (\"JBO\")\n                     @yo_yo_yo_jbo\n\x1b[0m-----------------------------------------------------------\n");
}

/************************************************************************
*                                                                       *
*  Function:   play                                                     *
*  Purpose:    Plays until the player quits.                            *
*  Parameters: - dictionary - the dictionary.                           *
*              - num_words - the number of words.                       *
*                                                                       *
*************************************************************************/
static
void
play(
    char* dictionary,
    size_t num_words
)
{
    char curr_word[WORD_LENGTH + 1] = { 0 };
    char curr_attempt[WORD_LENGTH + 1] = { 0 };
    int attempts = 0;
    bool won = false;

    // Play forever
    for (;;)
    {
        // Choose a new word randomly
        (void)strncpy(curr_word, dictionary + ((rand() % num_words) * WORD_LENGTH), sizeof(curr_word));

        // Play all rounds
        while (ATTEMPTS_NUM > attempts)
        {
            // Print game so far
            print_logo();

            // Get user input
            (void)printf("Enter your current attempt: ");
            (void)memset(curr_attempt, '\0', sizeof(curr_attempt));
            if (NULL == fgets(curr_attempt, sizeof(curr_attempt), stdin))
            {
                write_error("Failed reading user input.");
                goto cleanup;
            }
        }
    }

cleanup:

    // Return
    return;
}

/************************************************************************
*                                                                       *
*  Function:   read_entire_dictionary                                   *
*  Purpose:    Reads the entire dictionary file and returns it.         *
*  Returns:    The dictionary as a string upon success, NULL otherwise. *
*  Remarks:    Free the returned dictionary string upon success.        *
*                                                                       *
*************************************************************************/
static
char*
read_dictionary(void)
{
    char* result = NULL;
    char* dictionary = NULL;
    char buffer[PATH_MAX] = { 0 };
    FILE* fp = NULL;
    long file_size = 0;
    
    // Get self commandline file
    fp = fopen("/proc/self/cmdline", "r");
    if (NULL == fp)
    {
        write_error("Failed reading self commandline.");
        goto cleanup;
    }

    // Read entire file (cannot use ftell here due to procfs)
    if (fread(buffer, sizeof(*buffer), sizeof(buffer), fp) > sizeof(buffer) - 1)
    {
        write_error("Commandline is too long.");
        goto cleanup;
    }
    (void)fclose(fp);
    fp = NULL;

    // Validate commandline is absolute
    if ('/' != buffer[0])
    {
        write_error("Must use full path.");
        goto cleanup;
    }

    // Get the dictionary path
    *(strrchr(buffer, '/')) = '\0';
    (void)strncat(buffer, "/dictionary.txt", sizeof(buffer) - 1);

    // Open the dictionary path
    fp = fopen(buffer, "r");
    if (NULL == fp)
    {
        write_error("Failed reading dictionary.");
        goto cleanup;
    }

    // Conclude dictionary size
    if (-1 == fseek(fp, 0, SEEK_END))
    {
        write_error("Error reading dictionary file size.");
        goto cleanup;
    }
    file_size = ftell(fp);
    if ((0 >= file_size) || (MAX_DICTIONARY_SIZE < file_size) || (0 != (file_size % WORD_LENGTH)))
    {
        write_error("Dictionary is corrupted.");
        goto cleanup;
    }
    rewind(fp);
    
    // Allocate dictionary
    dictionary = calloc(file_size + 1, sizeof(*dictionary));
    if (NULL == dictionary)
    {
        write_error("Failed allocating dictionary buffer.");
        goto cleanup;
    }
    
    // Read entire dictionary
    if (fread(dictionary, sizeof(*dictionary), file_size, fp) != file_size)
    {
        write_error("Failed reading entire dictionary.");
        goto cleanup;
    }

    // Success
    result = dictionary;
    dictionary = NULL;

cleanup:

    // Free resources
    if (NULL != dictionary)
    {
        (void)free(dictionary);
        dictionary = NULL;
    }
    if (NULL != fp)
    {
        (void)fclose(fp);
        fp = NULL;
    }

    // Return result
    return result;
}

int
main()
{
    char* dictionary = NULL;

    // Print the logo
    print_logo();

    // Read the dictionary
    dictionary = read_dictionary();
    if (NULL == dictionary)
    {
        goto cleanup;
    }

    // Play the game
    play(dictionary, strlen(dictionary) / WORD_LENGTH);

cleanup:

    // Free resources
    if (NULL != dictionary)
    {
        (void)free(dictionary);
        dictionary = NULL;
    }

    // Always return 0
    return 0;
}

