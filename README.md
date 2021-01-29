# MCLIP-M
**High-Level Summary :** A simple but adequately featured and secured password manager for the command-line interfaces of ANY Operating System. 

**Detailed Description -**

MCLIP-M stands for "*Minimalist Command Line Interface Password-Manager*". 

It is a simple utility written 100% in C, and has no dependencies as it uses *only* the standard C library. 

It allows you to **generate random passwords or enter your own passwords and save them to a text file with comments and a time-stamp**. You can also **string-search the text file** in case you forget your passwords and it will display all lines with a matching sub-string. Alternatively, it can display everything in the text file. You may also be able to delete an entry in the *next release*. 

The utility has x86 binary executable releases for Windows, x86_64 static/dynamic binaries for Linux (compiled on *Ubuntu 20.04*) and x86_64 binaries for macOS but can be compiled and used on *any* OS with a standard C compiler and a terminal in a matter of seconds. In fact, it is *recommended* that you self-compile it if not using macOS or Windows, to ensure compatibility.

Accompanying this utility is a small Python script to help **encrypt the text file storing the passwords**. It has the following requirements : a UNIX-based OS, a Python3 Interpreter and GPG . However, you may change the encryption and decryption commands easily to use any other alternative to GPG. It must be run in an interactive terminal window, as it will require user inputs. In the future, this may be changed to a shell script, removing the need for a Python3 interpreter.

Together with the script, MCLIP-M is **sufficiently secure and adequately featured** for everyday use despite being **simple, minimal, hassle-free , light and having virtually no dependencies**.

MCLIP-M will be supported for some time and will be updated with new features.

You may request new features at apjo@tuta.io or tip the developer with Monero at 8BYAyzAZzLsZj7P6o2Y9FHDrViHprwevS429Q7KBQ5VY4VKuRN1T1eWNzwC9s7hvyygMne2Vn8QTB32yLRfsHBLFKH8vf3a . 

Note : How to install/use guides are part of the releases in PDF form.
