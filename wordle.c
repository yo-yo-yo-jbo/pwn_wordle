#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define WORD_LENGTH (6)
#define MAX_WORDS (10000)
#define MAX_DICTIONARY_SIZE (WORD_LENGTH * MAX_WORDS)

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

    // Allocate dictionary
    file_size = ftell(fp);
    if ((0 > file_size) || (MAX_DICTIONARY_SIZE < file_size))
    {
        write_error("Dictionary size is too big.");
        goto cleanup;
    }
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

    // Read the dictionary
    dictionary = read_dictionary();
    if (NULL == dictionary)
    {
        goto cleanup;
    }

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

