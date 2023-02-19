# MCLIP-M v3.4

 Minimalist Command Line Interface Password-Manager is a simple utility written in standard C99.

## Features :
1. Generates cryptographic-strength* random passwords of customizable length & character range.
2. Saves generated or other passwords with an automatic timestamp and optional comments as a CSV formatted entry to file. This makes it highly compatible across text editors, database managers, spreadsheet tools, and a whole host of advanced software should you wish to use it with them.
3. Find or delete entries in a file with case-insensitive substring search. 

*\*On POSIX systems or Windows XP and above only.*

## How to use :
1. [Download](https://download-directory.github.io/?url=https%3A%2F%2Fgithub.com%2Fa-p-jo%2FMCLIP-M%2Ftree%2Fmain%2Fv3) the v3 directory and unzip it.
2. Compile all `.c` files in the directory.  For example, on Linux/macOS do :
```sh
$ cc v3/*.c -Ofast -flto -o mclipm
```
3. Run the executable to see the help menu :
```
$ ./mclipm -h
MCLIPM v3.5 : Minimalist CLI Password Manager.
See https://github.com/a-p-jo/mclipm for git repo.

4 main commands : "g"(generate), "s"(save), "f"(find), "d"(delete)

1. "g" :
   Generates a random password.
   Example : mclipm g
          -> Displays a random 32-char (no spaces) password.
   Format  : mclipm g [option] <option's argument>...

   "g" may be given any combination of 3 options, in any order :

   1.1 "-l" : Specify length of password.
   Example  : mclipm g -l 16
           -> Resulting password is 16 chars long.
   Format   : mclipm g -l <n, {0 < n < SIZE_MAX-1}>...
   1.2 "-o" : Specify outfile and (optionally) comments.
   Example  : mclipm g -l 16 -o foo.csv "xyz.com, uname abc"
           -> Nothing is displayed except in case of errors.
           -> Appends CSV-formatted output to a new line in "foo.csv" :
              "Thu Jun 09 18:00:12 2016","a79&8*kjn!ok)$31","xyz.com, uname abc"
   Format   : mclipm g -o [filename (optional, default: "mclipm.csv")] [comments (optional, default: "")]...
   1.3 "-r" : Specify range of password's  ASCII characters.
              By default, the range is "!~", i.e. '~' >= char >= '!'.
              This allows all non-whitespace printable ASCII chars.
   Example  : mclipm g -r " "
           -> The lower limit is now ' '(space); password may include spaces.
   Format   : mclipm g -r "<lower_limit>[upper_limit(optional, default '~')]"

2. "s" :
   Save a password with optional comments, automatic timestamp; formatted as CSV
   Examples: mclipm s 12345
          -> Saves the password "12345" with empty ("") comments to mclipm.csv
   Format  : mclipm s <password> [comments] [filename (optional, default: "mclipm.csv")]

3. "f" :
   Find any entries in given file which matches given search terms.
   Example : mclipm f "" "x.com"
          -> Searches for entry with instance of "x.com" in default (mclipm.csv) file.
          -> Multiple terms, like "foo", "BAR" in "mclipm f x.csv foo BAR", also possible.
   Format  : mclipm f <filename (if empty, saves to mclipm.csv)> <search term>...

4. "d" : Same as "f"; except it deletes any entries with a match.
```
**Note :** 

Keeping passwords as plaintext is ill-advised. It is suggested that you  encrypt the file when not in use using your preffered
encryption tools for your OS/platform. 

For instance, you could use [XOTP](https://github.com/a-p-jo/xotp) with :
```sh
$ mclipm g -o /dev/stdout "new password" | xotp e -o passwords.xotp
```
