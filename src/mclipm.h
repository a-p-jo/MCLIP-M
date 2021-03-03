#include <stdio.h>  /* printf() fopen() fclose() fgets() scanf() perror() getchar() fflush()*/
#include <stdlib.h> /* srand() rand() */
#include <stdint.h> /* time_t size_t intptr_t */
#include <time.h>   /* ctime() time() */
#include <string.h> /* strstr() strlen() */

#define PSWD_LENGTH 32 /* Length of the password generated (in characters/bytes) */

#define PROGRAM_NAME "MCLIP-M v1.1"
#define PROGRAM_NAME_SIZE 12 /* not including null terminator, see LINE_BUFF_SIZE */

#define MAX_FILENAME_SIZE (1024+1) /* +1 for null terminator */
#define MAX_COMMENT_SIZE (2048+1)

/* Error codes */
#define FOPEN_ERR -1
#define FCLOSE_ERR -2
#define INAPPROPIATE_ARGS_ERR -3
#define NO_ARGS_ERR -4

/* Sensible size for a static buffer required to store a single line of the file we generate.
 * 
 * "PROGRAM_NAME : ctime() - '\n' : password : comment" is written to the file. 
 *
 * ctime() always returns 26 bytes, of which we ignore the null term and remove the newline, ergo 24.
 * 1024 or 1 KiB is the assumed max bytes in password; arbitrary - but arguably sufficient.
 * +10 is for the blanks and colons seperating each field. +1 is null terminator since buffer is a c string.
 */
#define LINE_BUFF_SIZE (PROGRAM_NAME_SIZE + 24 + 1024 + MAX_COMMENT_SIZE + 10 + 1) //The buffer for the search feature

/* Useful macros to avoid repeating boilerplate code */

#define EAT int eat; while ((eat = getchar()) != '\n' && eat != EOF); 
/* "eat" or read stdin to it's end (EOL/EOF) , discarding unused input (avoids it being read by unrelated input fields) */

#define GET_FILENAME printf("Filename : "); char filename[MAX_FILENAME_SIZE] = ""; fgets(filename,MAX_FILENAME_SIZE,stdin); rm_newline(filename);
/* get filename by printing a prompt, creating a buffer, reading string into buffer and removing fgets's stupid newline from it's end. */

#define GET_COMMENTS printf("Comments : "); char comments[MAX_COMMENT_SIZE]; fgets(comments,MAX_COMMENT_SIZE,stdin); rm_newline(comments);

/* Useful functions to avoid repeating boilerplate code */

/* Remove newline from the end of string (if it exists) : fixes fgets()'s newline when dealing with stdin. */
void rm_newline(char *);

/* Get current time as string.
 *
 * Wrapper that does ctime(&time(0)) that does two things :
 *
 * 1. Syntactic sugar, because derefrencing like &time(0) isn't possible, wasting lines.
 * 2. Removes the newline '\n' at the end of the string, as we will print it to file as one of many feilds in a line. 
 */
char * time_str(void);
