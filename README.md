# Terminal Wordle challenge
I was asked to write a challenge for the 2024 [Intent Summit](https://intentsummit.org) CTF.  
I ended up coding an interesting Linux challenge exploiting a [Wordle](https://en.wikipedia.org/wiki/Wordle) terminal implementation running as a SUID binary.
![Screenshot](screenshot.png)

## Running
The environment should consist of the flag, a file [dictionary.txt](dictionary.txt) and the binary `wordle.elf` installed as a SUID binary.  
Since this challenge was a part of the [Intent](https://intentsummit.org) CTF competition, we also prepared the Docker environment, which you can run as such:

```shell
docker-compose -f ./docker-compose.yml up
```

The goal of this challenge is to read the `flag` file.  

## Solution
The reader is strongly encouraged to solve on their own before reading the solution, which I will be describing here.

### The vulnerability
The challenge itsef presents a Wordle game. Although it's coded in C, there are no obvious memory corruption issues.  
In CTF challenges, things that might look odd are usually the right direction. In our case we have one - the `wordle.elf` binary only allows running it with a full commandline.  
Looking at the source code we can even see the explicit check under the function `read_dictionary`:

```c
static
char*
read_dictionary(void)
{
    ...

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

    ...

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

    ...
```

The code concludes the path of `dictionary.txt` from the commandline, and validates the commandline starts with a `/` character to ensure it's a full path!  
That is quite odd - a more natural approach is reading the symlink destination of `/proc/self/exe`, which does not depend on the commandline.  
Well, commandline can be spoofed - the easiest way to achieve this is with a symlink pointing to the `wordle.elf` path. Since the commandline will essentially be the symlink path, we control it completely.  
This means if I put a symlink under `/tmp` and link it to `wordle.elf`, the binary will be looking for a `/tmp/dictionary.txt`!  
Our next question is - what should we use as our `dictionary.txt` file? Well, since the binary reads it and we need the flag - the obvious solution would be putting `flag` there, but we can't since it's not readable. The solution is once again - a symlink, keeping in mind `fopen` follows symlinks by default.  
Note there is a check that ensures dictionary contents are divisible by 5, but lucky for us the `flag` file size is also divisible by 5, so we can proceed!

### Exploitation strategy
Now that we use our double-symlink idea, we essentially make `wordle.elf` use `flag` as the dictionary. Now we need to read it.  
The best approach is by *playing the game* - the game is implemented in a way that:
1. Lets you repeat guesses. You have 6 guesses per game, and you can repeat the same guess 6 times if you wish.
2. Tells you what the word was when you lose. We will be using that extensively as our read primitive, reading 5 characters from the `flag` file each time.
3. Forces you to attempt using a dictionary word. That means we have to have at least one word purely guesses from `flag`.
4. Words are case insensitive.

To solve #3, we could either bruteforce (with itertools) - assuming the normal alphabet, spaces, underscores, digits and curly brackets - we'd get `40^5 = 102400000` attempts, which is definitely feasible.   However, I've decided to just try the original dictionary first, assuming one word might hit. Indeed I got a hit - the word `THREE` is in the `flag` - excellent!  
At this point, I will use that word over and over again, losing on purpose every game and getting one more chunk of the `flag`.
Since the word is chosen randomly from the `flag` - I have to keep playing until I get enough chunks, and even then I still have to guess *the order* of those chunks.  
However, "luckily" those chunks make a complete and meaningful sentence, so that's totally acceptable.  

### The solution
I coded one solution under the `solve.py` file - it gets the directory of `wordle.elf` as an argument and:
1. Creates a symlink `sym` in a temporary directory that points to `wordle.elf`.
2. Creates a symlink `dictionary.txt` in the same directory that points to `flag`.
3. Uses the original `dictionary.txt` to run a dictionary attack against the game and discover one word (`THREE`).
4. Plays the game over and over, guessing `THREE` every time and getting one more chunk of the `flag`.
5. Finishes when the number of chunks indicate that the flag was fully read.

In a real CTF where people might be short on time, I expect step #3 to be done seperately.

An alternative to symlink creation in #1 is using the `exec` command which can freely manipulate the commandline.  
However, a symlink in #2 is still necessary.

## Summary
Although this exercise is easy, there are similar real world issues like that, involving symlink aatacks as well as partial reads of valuable data.

I hope you find this challenge and explanation useful!

Jonathan Bar Or
