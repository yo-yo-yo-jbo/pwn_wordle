/************************************************************************
*                                                                       *
*  File:       wordle.c                                                 *
*  Purpose:    Terminal-based game of Wordle.                           *
*                                                                       *
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

/************************************************************************
*                                                                       *
*  Constant:   WORD_LENGTH                                              *
*  Purpose:    Defines the word length.                                 *
*                                                                       *
*************************************************************************/
#define WORD_LENGTH (6)

/************************************************************************
*                                                                       *
*  Constant:   MAX_WORDS                                                *
*  Purpose:    Defines the maximum number of words.                     *
*                                                                       *
*************************************************************************/
#define MAX_WORDS (10000)

/************************************************************************
*                                                                       *
*  Constant:   MAX_DICTIONARY_SIZE                                      *
*  Purpose:    Defines the maximum dictionary size, in bytes.           *
*                                                                       *
*************************************************************************/
#define MAX_DICTIONARY_SIZE (WORD_LENGTH * MAX_WORDS)

/************************************************************************
*                                                                       *
*  Constant:   ATTEMPTS_NUM                                             *
*  Purpose:    Defines the number of attempts per round.                *
*                                                                       *
*************************************************************************/
#define ATTEMPTS_NUM (5)

/************************************************************************
*                                                                       *
*  Constant:   GREEN                                                    *
*  Purpose:    Defines the color green.                                 *
*                                                                       *
*************************************************************************/
#define GREEN ("\x1b[32m\x1b[1m")

/************************************************************************
*                                                                       *
*  Constant:   YELLOW                                                   *
*  Purpose:    Defines the color yellow.                                *
*                                                                       *
*************************************************************************/
#define YELLOW ("\x1b[33m\x1b[1m")

/************************************************************************
*                                                                       *
*  Constant:   RESET_COLOR                                              *
*  Purpose:    Defines the string that resets the color.                *
*                                                                       *
*************************************************************************/
#define RESET_COLOR ("\x1b[0m")

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
    (void)printf("\n\x1b[37m\x1b[1m  ██     ██   \x1b[31m\x1b[1m,d88b.d88b,\x1b[37m\x1b[1m  ██████  ██████  ██      ███████\n  ██     ██   \x1b[31m\x1b[1m88888888888\x1b[37m\x1b[1m  ██   ██ ██   ██ ██      ██     \n  ██  █  ██   \x1b[31m\x1b[1m`Y8888888Y\'\x1b[37m\x1b[1m  ██████  ██   ██ ██      █████  \n  ██ ███ ██     \x1b[31m\x1b[1m`Y888Y\'\x1b[37m\x1b[1m    ██   ██ ██   ██ ██      ██     \n   ███ ███        \x1b[31m\x1b[1m`Y\'\x1b[37m\x1b[1m      ██   ██ ██████  ███████ ███████\n\n             Level by Jonathan Bar Or (\"JBO\")\n                     @yo_yo_yo_jbo\n\x1b[0m\n\n");
}

/************************************************************************
*                                                                       *
*  Function:   print_attempt                                            *
*  Purpose:    Prints an attempt.                                       *
*  Parameters: - attempt - the attempt, assumed to be a valid word.     *
*              - word - the correct word, assumed to be a valid word.   *
*  Returns:    True if the word and the attempt match, false otherwise. *
*                                                                       *
*************************************************************************/
static
bool
print_attempt(
    char* attempt,
    char* word
)
{
    char* colors[WORD_LENGTH] = { NULL };
    size_t counter = 0;
    int frequencies[256] = { 0 };
    int hits = 0;

    // Get letter frequencies
    for (counter = 0; counter < WORD_LENGTH; counter++)
    {
        frequencies[toupper(word[counter])]++;
    }

    // Finds the correct letters and assigns green to them
    for (counter = 0; counter < WORD_LENGTH; counter++)
    {
        if (toupper(word[counter]) == toupper(attempt[counter]))
        {
            colors[counter] = GREEN;
            frequencies[toupper(word[counter])]--;
            hits++;
        }
    }

    // Finds misplaced letters
    for (counter = 0; counter < WORD_LENGTH; counter++)
    {
        if ((NULL == colors[counter]) && (0 < frequencies[toupper(attempt[counter])]))
        {
            colors[counter] = YELLOW;
            frequencies[toupper(attempt[counter])]--;
        }
    }

    // Print the result
    for (counter = 0; counter < WORD_LENGTH; counter++)
    {
        printf("%s%c%s", colors[counter] != NULL ? colors[counter] : "", toupper(attempt[counter]), RESET_COLOR);
    }
    printf("\n");

    // Indicate success
    return WORD_LENGTH == hits;
}

/************************************************************************
*                                                                       *
*  Function:   play_one_round                                           *
*  Purpose:    Plays one round.                                         *
*  Parameters: - dictionary - the dictionary.                           *
*              - num_words - the number of words.                       *
*  Returns:    True if the round ended well, false upon errors.         *
*                                                                       *
*************************************************************************/
static
bool
play_one_round(
    char* dictionary,
    size_t num_words
)
{
    bool result = false;
    char curr_word[WORD_LENGTH + 1] = { 0 };
    char curr_attempt[WORD_LENGTH + 2] = { 0 };
    char past_attempts[ATTEMPTS_NUM][WORD_LENGTH + 1] = { 0 };
    unsigned long attempts = 0;
    unsigned long counter = 0;
    bool won = false;
    
    // Choose a new word randomly
    (void)strncpy(curr_word, dictionary + ((rand() % num_words) * WORD_LENGTH), sizeof(curr_word) - 1);

    // Play all rounds
    while (ATTEMPTS_NUM > attempts)
    {
        // Print game so far
        print_logo();
        for (counter = 0; counter < attempts; counter++)
        {
            (void)print_attempt(past_attempts[counter], curr_word);
        }

        // Get user input
        (void)printf("Enter your current attempt: ");
        (void)memset(curr_attempt, '\0', sizeof(curr_attempt));
        if (NULL == fgets(curr_attempt, sizeof(curr_attempt) - 1, stdin))
        {
            write_error("Failed reading user input.");
            goto cleanup;
        }
        if (('\n' == curr_attempt[0]) || ('\r' == curr_attempt[0]))
        {
            continue;
        }
        
        // Validate input
        curr_attempt[WORD_LENGTH] = '\0';

        // Check the attempt
        if (print_attempt(curr_attempt, curr_word))
        {
            won = true;
            break;
        }

        // Save the attempt as a past attempt
        (void)strncpy(past_attempts[attempts], curr_attempt, sizeof(curr_attempt));
        attempts++;
    }

    // Print solution
    if (won)
    {
        printf("\nGreat job on guessing the word %s%s%s!\n", GREEN, curr_word, RESET_COLOR);
    }
    else
    {
        printf("\nYou failed guessing the word %s%s%s.\n", RED, curr_word, RESET_COLOR);
    }

    // Success
    result = true;

cleanup:

    // Return result
    return result;
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
    unsigned long rounds = 0;
    unsigned long wins = 0;
    int choice = 0;
    bool quit = false; 
 
    // Play forever
    while (!quit)
    {
        // Play a round
        if (!play_one_round(dictionary, num_words))
        {
            goto cleanup;
        }

        // Optionally play again
        do
        {
            if (('\n' != choice) && ('\r' != choice))
            {
                printf("Play again [Y/N]: ");
            }
            choice = getchar();
            if (('N' == choice) || ('n' == choice))
            {
                quit = true;
                break;
            }
        }
        while (('Y' != choice) && ('y' != choice));
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

/************************************************************************
*                                                                       *
*  Function:   main                                                     *
*  Purpose:    Main routine.                                            *
*  Returns:    Zero, always.                                            *
*                                                                       *
*************************************************************************/
int
main()
{
    char* dictionary = NULL;

    // No buffering
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

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

