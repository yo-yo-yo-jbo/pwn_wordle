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
#define WORD_LENGTH (5)

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
#define ATTEMPTS_NUM (6)

/************************************************************************
*                                                                       *
*  Constant:   RED                                                      *
*  Purpose:    Defines the color red.                                   *
*                                                                       *
*************************************************************************/
#define RED ("\x1b[31m\x1b[1m")

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
*  Constant:   GRAY                                                     *
*  Purpose:    Defines the color gray.                                  *
*                                                                       *
*************************************************************************/
#define GRAY ("\x1b[37m\x1b[2m\x1b[1m")

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
    (void)printf("%sERROR:%s %s\n", RED, RESET_COLOR, msg);
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
*  Function:   check_attempt                                            *
*  Purpose:    Checks an attempt and optionally prints it.              *
*  Parameters: - attempt - the attempt, assumed to be a valid word.     *
*              - word - the correct word, assumed to be a valid word.   *
*              - print - whether to print or not.                       *
*  Returns:    True if the word and the attempt match, false otherwise. *
*  Remarks:    - Case insensitive.                                      *
*                                                                       *
*************************************************************************/
static
bool
check_attempt(
    char* attempt,
    char* word,
    bool print
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

    // Find misplaced letters
    for (counter = 0; counter < WORD_LENGTH; counter++)
    {
        if ((NULL == colors[counter]) && (0 < frequencies[toupper(attempt[counter])]))
        {
            colors[counter] = YELLOW;
            frequencies[toupper(attempt[counter])]--;
        }
    }

    // Print the result
    if (print)
    {
        for (counter = 0; counter < WORD_LENGTH; counter++)
        {
            printf("%s%c%s", colors[counter] != NULL ? colors[counter] : "", toupper(attempt[counter]), RESET_COLOR);
        }
        printf("\n");
    }

    // Indicate success
    return WORD_LENGTH == hits;
}

/************************************************************************
*                                                                       *
*  Function:   is_dictionary_word                                       *
*  Purpose:    Checks if the given word exists in the dictionary.       *
*  Parameters: - word - the given word.                                 *
*              - dictionary - the dictionary.                           *
*  Returns:    True if and only if word is found in the dictionary.     *
*  Remarks:    - Case insensitive.                                      *
*                                                                       *
*************************************************************************/
static
bool
is_dictionary_word(
    char* word,
    char* dictionary
)
{
    char* ptr = NULL;
    char* end = NULL;
    size_t counter = 0;
    bool failed = false;

    // Iterate all dictionary words
    end = dictionary + strlen(dictionary);
    for (ptr = dictionary; ptr < end; ptr += WORD_LENGTH)
    {
        failed = false;
        for (counter = 0; counter < WORD_LENGTH; counter++)
        {
            if (toupper(word[counter]) != toupper(ptr[counter]))
            {
                failed = true;
                break;
            }
        }

        // Quit if found
        if (!failed)
        {
            break;
        }
    }

    // Indicate result
    return !failed;
}

/************************************************************************
*                                                                       *
*  Function:   print_game_header                                        *
*  Purpose:    Prints out the game's header.                            *
*  Parameters: - last_error_message - an optional error message.        *
*              - attempts - the number of attempts so far.              *
*              - curr_word - the current target word.                   *
*              - past_attempts - the past attempts.                     *
*              - number_of_dict_words - number of dictionary words.     *
*                                                                       *
*************************************************************************/
static
void
print_game_header(
    char* last_error_message,
    unsigned long attempts,
    char* curr_word,
    char past_attempts[ATTEMPTS_NUM][WORD_LENGTH + 1],
    unsigned long number_of_dict_words
)
{
    size_t counter = 0;

    // Print logo and last error message
    print_logo();
    (void)printf("%sDictionary contains %lu words.%s\n", GRAY, number_of_dict_words, RESET_COLOR);
    if (NULL != last_error_message)
    {
        (void)printf("%sERROR: %s%s\n\n", RED, RESET_COLOR, last_error_message);
    }
    else
    {
        (void)printf("\n\n");
    }

    // Print all attempts
    for (counter = 0; counter < ATTEMPTS_NUM; counter++)
    {
        printf("%s%lu.%s ", RED, counter + 1, RESET_COLOR);
        if (counter < attempts)
        {
            if (check_attempt(past_attempts[counter], curr_word, true))
            {
                goto cleanup;
            }
        }
        else
        {
            (void)printf("%s\n", counter == attempts ? "<<< CURRENT ATTEMPT >>>" : "");
        }
    }

cleanup:

    // Return
    return;
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
    bool won = false;
    char* last_error_message = NULL;
    char* newline = NULL;
    unsigned long number_of_dict_words = 0;    

    // Conclude number of dictionary words
    number_of_dict_words = strlen(dictionary) / WORD_LENGTH;

    // Choose a new word randomly
    (void)strncpy(curr_word, dictionary + ((rand() % num_words) * WORD_LENGTH), sizeof(curr_word) - 1);

    // Play all rounds
    while (ATTEMPTS_NUM > attempts)
    {
        // Print game so far
        print_game_header(last_error_message, attempts, curr_word, past_attempts, number_of_dict_words);

        // Get user input
        (void)printf("Enter your current attempt: ");
        (void)memset(curr_attempt, '\0', sizeof(curr_attempt));
        if (NULL == fgets(curr_attempt, sizeof(curr_attempt) - 1, stdin))
        {
            write_error("Failed reading user input.");
            goto cleanup;
        }        
       
        // Get rid of newline leftovers by fgets
        curr_attempt[WORD_LENGTH] = '\0';
        newline = strchr(curr_attempt, '\n');
        if (NULL != newline)
        {
            *newline = '\0';
        }
        newline = strchr(curr_attempt, '\r');
        if (NULL != newline)
        {
            *newline = '\0';
        }
        if ('\0' == curr_attempt[0])
        {
            continue;
        }
        
        // Validate input
        last_error_message = NULL;
        if (WORD_LENGTH != strlen(curr_attempt))
        {
            last_error_message = "Word length is incorrect.";
            continue;
        }
        if (!is_dictionary_word(curr_attempt, dictionary))
        {
            last_error_message = "Word is not a dictionary word.";
            continue;
        }

        // Save the attempt as a past attempt
        (void)strncpy(past_attempts[attempts], curr_attempt, WORD_LENGTH + 1);
        attempts++;

        // Check the attempt
        if (check_attempt(curr_attempt, curr_word, false))
        {
            won = true;
            break;
        }
    }

    // Print solution
    print_game_header(last_error_message, attempts, curr_word, past_attempts, number_of_dict_words);
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
            // Get rid of newline leftovers by getchar
            if (('\n' != choice) && ('\r' != choice))
            {
                printf("Play again [Y/N]: ");
            }

            // Get the choice
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

    // Set buffering
    setvbuf(stdin, NULL, _IOFBF, 0);
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

