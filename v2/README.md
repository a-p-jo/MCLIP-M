# MCLIP-M v2

MCLIP-M, Minimalist Command Line Interface Password-Manager, is a simple utility written purely in standard C99.

## Features :
1. Generates pseudo-random passwords consisting of printable, non-whitespace characters. Default lenght is 32 characters, although it can be easily customized by editing the macro.
2. Saves generated or other passwords with an automatic timestamp and optional comments as a CSV formatted entry to file. This makes it highly compatible across text editors, database managers,spreadsheet tools, and a whole host of advanced software should you wish to use it with them.
3. Search or delete entries in a file that match in a case-insensitive string-search with search terms you enter. 

## How to use :
```c
/* How to Use :
 * 0. Download the .c file, compile, and run in a terminal/command prompt with any of the below options :
 * 1. Option : g,generate            Usage : mclipm g        
 * 2. Option : g & s,generate & save Usage : mclipm g s <filename>
 * 3. Option : f,find                Usage : mclipm f <filename> <search_term> [search_term]...
 * 4. Option : d,delete              Usage : mclipm d <filename> <search_term> [search_term]...
 * 5. Option : r,read                Usage : mclipm r <filename>
 */
 ```
**Notes :** 
1. Any version of MCLIP-M can be compiled and used on any OS with a standard C compiler and terminal in seconds.
2. Keeping passwords as plaintext is ill-advised. It is suggested that you  encrypt the file when not in use using your favorite
encryption tools for your OS/platform.
