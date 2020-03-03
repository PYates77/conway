# conway
A simple in-terminal conway's game of life implementation

### conway.c
This is my first draft implementation, contains comments and relatively clean helper functions

### conway-obfusc.c
A minimized and obfuscated version, used to challenge friends to see if they can figure out what it does without running it

## Building
As per usual, I used curses in order to avoid having to use an actual graphics library, plus I like filling the terminal with stuff

`gcc conway.c -o conway -lcurses`
