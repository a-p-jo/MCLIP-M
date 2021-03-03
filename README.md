# MCLIP-M v1.1 

MCLIP-M, Minimalist Command Line Interface Password-Manager, is a simple utility written 100% in C; It has **no dependencies as it uses only the standard C library**. 

## Features :
- **Generation :** Generate pseudo-random passwords consisting of *all* characters. Default size is 32 characters, this can be customized easily.

- **Saving :** Save the generated passwords as well as your other passwords on a text file with comments and an automatic time-stamp. Being in a text file makes it very portable and easy to view and edit as your passwords change. The time stamp and comments will help give context to the password in the future.

- **Look-up :** A simple yet effective string-search option, if you forget your password details and have too many to search by reading the whole file. Also, as data is stored in a text file, more sophisticated searches can also be performed with your favorite text editor.

- **Security :** Saving passwords in plaintext is bad, and we recommend encrypting the file when not in use. Published with MCLIP-M is a Python script that can help you encrypt your file easily on UNIX-based systems. By default it uses GPG to encrypt with a password, and this is adequate for everyday use. However, the encryption/decryption commands can be easily changed to work with other OSes, tools and cryptographic techniques.

- **Simplicity :** No hundreds of command-line options, no worrisome dependencies,no confusing and application-specific binary database files. Easy to compile to any OS, all the features that are really necessary, data saved as easily viewable and editable text files and just ~200 lines of C code. Minimal, fast, portable, effective, and reliable. 

**Notes :** 
1. Any version of MCLIP-M can be compiled and used on any OS with a standard C compiler and terminal in seconds.
2. The Python script must be run in an interactive terminal window, as it will require user inputs.
