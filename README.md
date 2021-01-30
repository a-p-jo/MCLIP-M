# MCLIP-M : Minimalist Command Line Interface Password-Manager  

MCLIP-M is a simple utility written 100% in C; It has **no dependencies as it uses only the standard C library**. 

## Features :
- **Generation :** Generate pseudo-random passwords consisting of *all* characters. Default size is 32 characters, this can be customized easily.

- **Saving :** Save the generated passwords as well as your other passwords on a text file with comments and an automatic time-stamp. Being in a text file makes it very portable and easy to view and edit as your passwords change. The time stamp and comments will help give context to the password in the future.

- **Look-up :** A simple yet effective string-search option, if you forget your password details and have too many to search by reading the whole file. Also, as data is stored in a text file, more sophisticated searches can also be performed with your favorite text editor.

- **Security :** Saving passwords in plaintext is bad, and we recommend encrypting the file when not in use. Published with MCLIP-M is a Python script that can help you encrypt your file easily on UNIX-based systems. By default it uses GPG to encrypt with a password, and this is adequate for everyday use. However, the encryption/decryption commands can be easily changed to work with other OSes, tools and cryptographic techniques.

- **Simplicity :** No hundreds of command-line options, no worrisome dependencies,no confusing and application-specific binary database files. Easy to compile to any OS, all the features that are really necessary, data saved as easily viewable and editable text files and just ~200 lines of C code. Minimal, fast, portable, effective, and reliable. 

## Versions :
- "Interactive" : Doesn't take command-line arguments; asks for inputs one-by-one. Ideal for those not savvy with a Terminal or paradigms where point-and-click is common.

- "Precise" : Takes all input as command-line arguments at once. Fast, efficient , precise and meant for Terminal-savvy users who find point-and-click tedious and shallow.

## Binary Release (Upcoming) Information :
- **Windows :** x86 Executables for Windows 7 and higher ; MCLIP-M "Interactive" only (Takes no command-line arguments) + How-To ; MSVC-compiled on Windows 7, VS2019.

- **macOS :** x86_64 Binaries ; MCLIP-M Interactive + MCLIP-M "Precise" (Takes command-line arguments) + Security.py + How-To ; Clang-compiled on macOS 11

- **Linux :** x86_64 Static & Dynamic Binaries ; MCLIP-M Precise only + Security.py + How-To ; GCC-compiled on Ubuntu 20.04 

Of course, any version of MCLIP-M can be compiled and used on any OS with a standard C compiler and a terminal in a matter of seconds. In fact, it is recommended that you self-compile it if possible, to ensure compatibility.

Accompanying this utility is a small Python script to help encrypt the text file storing passwords. It must be run in an interactive terminal window, as it will require user inputs.

**Suggest features or report bugs to apjo@tuta.io or tip the developer (Monero) at 8BYAyzAZzLsZj7P6o2Y9FHDrViHprwevS429Q7KBQ5VY4VKuRN1T1eWNzwC9s7hvyygMne2Vn8QTB32yLRfsHBLFKH8vf3a**
