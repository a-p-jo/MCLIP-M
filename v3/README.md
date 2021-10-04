# MCLIP-M v3.11

 Minimalist Command Line Interface Password-Manager is a simple utility written purely in standard C99.

## Features :
1. Generates cryptographic-strength* random passwords of customizable length & character range.
2. Saves generated or other passwords with an automatic timestamp and optional comments as a CSV formatted entry to file. This makes it highly compatible across text editors, database managers,spreadsheet tools, and a whole host of advanced software should you wish to use it with them.
3. Find or delete entries in a file with case-insensitive search. 

*\*On POSIX-compliant operating systems or Windows XP and above only.*

## How to use :
1. [Download](https://download-directory.github.io/?url=https%3A%2F%2Fgithub.com%2Fa-p-jo%2FMCLIP-M%2Ftree%2Fmain%2Fv3) the v3 directory and unzip it.
2. Compile all `.c` files in the directory.  For example, on Linux/macOS do :
```sh
$ gcc v3/primitives.c v3/io.c v3/main.c -O3 -flto -o mclipm
```
3. Run the executable with no options to see the help menu :
```
$ ./mclipm
MCLIPM v3 : Minimalist Command-Line Password Manager.
See https://github.com/a-p-jo/mclipm for git repo.

4 main commands : "g"(generate), "s"(save), "f"(find), "d"(delete)

1. "g" :
   Generates a random password.
   Example : mclipm g
          -> Generates and output a password containing 32 non-whitespace printable ASCII chars.
   Format  : mclipm g [options (optional)] [option's argument (optional)]...

   Optionally, "g" takes any of 3 options, in any order :

   1.1 "-l" : Specify the number of characters in the password.
   Example  : mclipm g -l 16
           -> Outputs a 16-char password.
   Format   : mclipm g -l <any natural number less than 2^64 - 2>...
   1.2 "-o" : Save password, optional comments and automatic timestamp as CSV entry to a file.
   Example  : mclipm g -l 16 -o foo.csv "This is a comment"
           -> Nothing is displayed except in case of errors.
           -> Appends the 16-char password to "foo.csv", like so :
              "Thu Jun 09 18:00:12 2016","a79&8*kjn!ok)$31","This is a comment"
   Format   : mclipm g -o [filename (optional, default: "mclipm.csv")] [comments (optional, default: "")]...
   1.3 "-r" : Specify range of password's  ASCII characters.
              By default, the range is "!~", i.e. for any char, '~' >= char >= '!'.
              This allows any non-whitespace printable (English) chars. You may consult a chart.
   Example  : mclipm g -r " "
           -> The lower limit char is now ' '(space); password may include spaces.
   Format   : mclipm g -r "<lower_limit>[upper_limit(optional, default '~')]"

2. "s" :
   Save a password with optional comments, automatic timestamp & encoded as CSV
   Example : mclipm s 12345
          -> Saves the password "12345" with empty ("") comments to mclipm.csv
             mclip s 1234 "strong & complicated password" "donread.csv"
          -> Saves the same password but with "strong..." comment and to "donread.csv"
   Format  : mclipm s <password> [comments (optional)] [filename (optional, mclipm.csv default)]

3. "f" :
   Searches for an entry case-insensitively with sub-string matching, and displays it.
   Example : mclipm f "" "foo@gmail.com"
          -> Searches for entry with instance of foo@gmail.com in default (mclipm.csv) file.
          -> Multiple search terms, say like "mclipm f x.csv gmail arch", also possible.
   Format  : mclipm f <filename (if empty, saves to mclipm.csv)> <search term> [search term]...

4. "d" : Same as "f"; except it deletes an entry with a match.
```
**Note :** 

Keeping passwords as plaintext is ill-advised. It is suggested that you  encrypt the file when not in use using your favorite
encryption tools for your OS/platform. 

For instance, you could use [XOTP](https://github.com/a-p-jo/xotp) with :
```sh
$ mclipm g -o /dev/stdout "new password" | xotp e -o passwords.xotp
```
