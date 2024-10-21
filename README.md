# Terminal Wordle challenge
An interesting Linux challenge exploiting a [Wordle](https://en.wikipedia.org/wiki/Wordle) terminal implementation running as a SUID binary.
![Screenshot](screenshot.png)

## Running
The environment should consist of the flag, a file [dictionary.txt](dictionary.txt) and the binary `wordle` installed as a SUID binary.  
To compile and set up the correct permissions, simply run:

```shell
sudo ./compile.sh
```

The goal is reading `flag.txt` by exploiting the `wordle` binary.  
Note that the `wordle` binary can only run when supplied as a full path. For convenience, the script `run_me.sh` is supplied, simply run it:

```shell
./run_me.sh
```

The reader is strongly encouraged to solve on their own before reading the solution, which I will be describing here.
