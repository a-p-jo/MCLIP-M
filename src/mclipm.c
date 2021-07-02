#include <stdio.h>  /* fopen,fclose,FILE,fgets,fputs,puts,printf,perror,strerror,fprintf */
#include <stdlib.h> /* malloc,free,exit,EXIT_SUCCESS */
#include <errno.h>  /* errno */
#include <time.h>   /* time,ctime,time_t */
#include <stdint.h> /* uint8_t,uint64_t,uint_fast8_t */
#include <string.h> /* strstr,strlen */
#include <ctype.h>  /* tolower() */

/* Convenient typedefs */ 
typedef uint8_t      u8;
typedef uint64_t     u64;
typedef uint_fast8_t uf8;

/* Constants */
#define DEFAULT_PWD_LEN   32
#define TMP_FNAME         ".temporary.mclipm"

/* Error Exit Codes */
#define FOPEN_ERR         -1
#define FCLOSE_ERR        -2
#define INAPPROPIATE_ARGS -3
#define NOT_ENOUGH_ARGS   -4
#define WRITE_ERR         -5
#define SEEK_ERR          -6
#define MALLOC_ERR        -7
#define READ_ERR          -8
#define RENAME_ERR        -9

/* Functions */

/* 1. Removes newline, if present, from string, including CRLF or even just CR*/
char * rm_newline(char * str)
{
        size_t len = strlen(str);
        if(len>=2 && str[len-2] == '\r') str[len-2] = '\0';
        else if(len>=1 && (str[len-1] == '\n' || str[len=1] == '\r') ) str[len-1] = '\0';
        return str;
}

/* 2. Returns current time as a string */
char * time_str(void)
{
        time_t time_num = time(0);
        char * time_str = ctime(&time_num);
        
        if(time_str) return rm_newline(time_str);
        else return NULL;
}

/* 3. Returns given string formatted per CSV conventions on quotes
 *    -> Always adds quotes around string
 *    -> Returned string must be free()'d
 */
char * CSV_format(char * str)
{
        /* Iterate over string to obtain it's length and number of quotes */
        size_t len, quotes;
        for(len = 0, quotes = 0; str[len]; len++)
                if(str[len] == '"') quotes++;

        char * rval;

        /* Allocate memory for resulting string, enough to store :
         *  original string + as many quotes as in original string + quotes in the beginning and ending of string + '\0' 
         */
        if(!(rval = malloc(len + quotes + 2 + 1)))
        {
                perror("Error : Could not allocate sufficient memory ");
                exit(MALLOC_ERR);
        }

        /* 1. Add quotes to beginning of resulting str 
         * 2. For every quote in string, add one exta quote to resultant string. Copy all chars to resultant string.
         * 3. Append quotes and '\0' to resulting string, and return it.
         */
        rval[0] = '"';
        size_t i,q;
        for(i = 0, q = 1; str[i]; rval[q++] = str[i++])
                if(str[i] == '"') rval[q++] = '"';
        rval[q++] = '"'; rval[q] = '\0';
        return rval;
}

/* 4. Writes CSV formatted entry */
void write_pswd(char * fname, FILE * fp, char * comments, char * pswd)
{
        /* Steps :
         * 1. Get time string
         * 2. CSV format all strings (comments,passwrord,time) as CSV fields.
         * 3. Write entry to fp seperated by ',' and flush fp.
         * 4. Print entry.
         */

        /* If time_str returns NULL, time is an empty string (no time). Else, use the time string returned    */
        char * time; (time = time_str())? (time = time) : (time = "");
        pswd = CSV_format(pswd); time = CSV_format(time); comments = CSV_format(comments);

        /* Because fprintf() returns int, wider type would not help. */
        int should, did;
        /* Check that bytes actually written are equal to bytes that ought to be written. */
        if((did = fprintf(fp,"%s,%s,%s\n",time,pswd,comments)) !=
         (should = (strlen(time) + strlen(pswd) + strlen(comments) + strlen(",,\n"))))
        {
                fprintf(stderr,"Couldn't properly write entry to file, %d of %d characters written.\n",did,should);
                free(pswd); free(time); free(comments);
		exit(WRITE_ERR);
        }
        /* Check that entry could actually be written to file */
        else if(fflush(fp))
        {
                perror("Error : Couldn't save entry to file ");
		free(pswd); free(time); free(comments);
		exit(WRITE_ERR);
        }
        else printf("%s : %s,%s,%s\n",fname,time,pswd,comments);
        free(pswd); free(time); free(comments);
}

/* 5. Return given string with all lowercase letters 
 *    -> Returned string must be free()'d
 */
char * lower(char *str)
{    
        char * rval = malloc(strlen(str)+1);

        if(!rval)
        {
                perror("Error : Not enough memory for string ");
                exit(MALLOC_ERR);
        }
        else for(char * ptr = rval;; ptr++, str++)
        {
                *ptr = tolower(*str);
                if(*ptr == '\0') return rval;
        }
}

int main(int argc, char **argv)
{
        /* Options :
         * 1. "g" : Generate random password of DEFAULT_PWD_LEN non-whitespace characters. 
         *    Optionally, if specified, appended as CSV entry to given file. 
         *    Entry contains 3 felids : time, password, and comments.
         * 2. "s" : Save given password and optionally, comments, as CSV entry to given file.
         * 3. "f" : Find any instance(s) of given string(s) in any line of given file.
         *    Print line(s) with match(es) along with their line number(s). Also display frequency of matches.
         * 4. "d" : Delete any line(s) with instance(s) of given string(s) from given file.
         */
              
        if(argc < 2)
        {
                fputs("Error : No command line arguments.\n",stderr);
                exit(NOT_ENOUGH_ARGS);
        }
        switch(*argv[1])
        {
                case 'g' : case 'G' :
                {
                        /* Steps :
                         * 1. Generate null-terminated string with DEFAULT_PWD_LEN random, non-whitespace chars.
                         * 2. If asked to save said password, append a CSV-formatted entry with current time, the password, as well as comments(if any) and print entry.
                         * 3. Else , print password.
                         */

                        srand(time(0));
                        char pswd[DEFAULT_PWD_LEN+1] = ""; 

                        #define UPPER_LIM '~' /* 126 */
                        #define LOWER_LIM '!' /* 41  */

                        for(uf8 i = 0; i < DEFAULT_PWD_LEN; i++)
                                pswd[i] = (rand() % (UPPER_LIM - LOWER_LIM + 1)) + LOWER_LIM;
                        
                        pswd[DEFAULT_PWD_LEN] = '\0';

                        /* if argv[2] is non-NULL, and it's first char is 's' or 'S', save the given password. Else, display and exit */
                        if((argv[2] && (*argv[2] == 's' || *argv[2] == 'S')))
                        {
                                if(argc < 4 )
                                {
                                        /* If unsaved, display password, error message and exit */
                                        puts(pswd);
                                        fputs("Not enough arguments.\nFormat for \"g\" option with \"s\" argument :\n\tmclipm g [s] [<file>] [comments]\n",stderr);
                                        exit(NOT_ENOUGH_ARGS);
                                }

                                char * fname = argv[3];
                                FILE * fp = fopen(fname,"a");
                                if(!fp)
                                {
                                        puts(pswd);
                                        perror("Error : Couldn't open file to append ");
                                        exit(FOPEN_ERR);
                                }
                                write_pswd(fname,fp, argv[4]? argv[4] : "", pswd);
                                if(fclose(fp))
                                {
                                        perror("Error : Could not close file ");
                                        exit(FCLOSE_ERR);
                                }
                        }
                        else puts(pswd);
                        
                        break;
                }
                case 's' : case 'S' :
                {
                        /* Steps :
                         * 1. Append all given passwords & comments with timestamp as CSV entry to given file
                         */
                        if(argc < 4 )
                        {
                                fputs("Not enough arguments.\nFormat for \"s\" option :\n\tmclipm s <file> <password> [comments] ...\n",stderr);
                                exit(NOT_ENOUGH_ARGS);
                        }
                        char * fname = argv[2];
                        FILE * fp    = fopen(fname,"a");
                        if(!fp)
                        {
                                perror("Error : Couldn't open file to append ");
                                exit(FOPEN_ERR);
                        }

                        for(unsigned i = 3; i < argc; i += 2)
                                write_pswd(fname,fp, argv[i+1], argv[i]);

                        if(fclose(fp))
                        {
                                perror("Error : Could not close file ");
                                exit(FCLOSE_ERR);
                        }

                        break;
                }
                /* Find and delete share core mechanisms of line-by-line string search; single block can be be used for both. */
                case 'f' : case 'F' : case 'd' : case 'D' :
                {
                        /* Steps :
                         * 1. Read a line from the file and search it without case-sensitivity for first instance of any given string.
                         * 2. If matched, print the line along with it's line number , and if deleting, don't write it to temporary file.
                         * 3. Else, if deleting, write line to temporary file.
                         * 4. Once file ends, print number of matched found and if deleting, rename temporary file to given file.
                         */

                        unsigned deleting = (*argv[1] == 'd' || *argv[1] == 'D');

                        if(argc < 4)
                        {
                                fprintf(stderr,
                                "Not enough arguments.\nFormat for \"%s\" option :\n\tmclipm %s <file> <string> ...\n",
                                deleting? "d" : "f", deleting? "d(elete)" : "f(ind)");
                                exit(NOT_ENOUGH_ARGS);
                        }

                        char * fname = argv[2];
                        FILE * fp    = fopen(fname,"r");
                        if(!fp)
                        {
                                perror("Error : Couldn't open file to read ");
                                exit(FOPEN_ERR);
                        }                       
                        FILE * tmp = NULL;
                        if(deleting)
                        {
                                tmp = fopen(TMP_FNAME,"w");
                                if(!tmp)
                                {
                                        perror("Error : Couldn't open temporary file to write changes ");
                                        exit(FOPEN_ERR);
                                }
                        }
                        u64 count = 0;

                        /* Read line and search for given string(s) */
                        for(u64 line = 1;;line++)
                        {
                                size_t bytes = 0;
                                for(int c; (c = fgetc(fp)) != '\n'; bytes++) if(c == EOF) 
                                {
                                        if(bytes) break;
                                        else goto end; /* Nothing left to read, end reading */
                                }
                                /* In above loop, we iterate bytes+1 times, reading bytes+1 characters
                                * since only the failing fgetc terminates the loop. Hence, we must rewind that many chars
                                */
                                if(fseek(fp,-(bytes+1),SEEK_CUR))
                                {
                                        perror("Error : Couldn't reverse seek file ");
                                        exit(SEEK_ERR);
                                }

                                /* In allocating memory, account for null terminator as well as newline */
                                char * buf = malloc(bytes+2);
                                if(!buf)
                                {
                                        perror("Error : Couldn't allocate enough memory ");
                                        exit(MALLOC_ERR);
                                }

                                if(!fgets(buf,bytes+2,fp))
                                {
                                        fputs("Error : Couldn't read lines from file.\n",stderr);
                                        free(buf);
					exit(READ_ERR);
                                }
                                                              
                                char * found = NULL; /* Both state variable and pointer */
                                for(unsigned i = 3; i < argc; i++)
                                {
                                        char * case_insensitive_buf = lower(buf), * case_insensitive_argv = lower(argv[i]);

                                        if( ( found = strstr(case_insensitive_buf,case_insensitive_argv) ) ) i = argc;
                                        
                                        free(case_insensitive_buf); free(case_insensitive_argv);
                                }

                                if(found)
                                {
                                        printf("Line %lld\t%s\n", line, rm_newline(buf));
                                        count++;
                                }
                                else if(deleting && (fputs(buf,tmp) == EOF))
                                {
                                        perror("Error : Couldn't write to file ");
                                        free(buf);
					exit(WRITE_ERR);
                                }
                                free(buf);
                        }

                        end :
                        printf("%llu match%s found.\n",count,count == 1? "" : "es");

                        if(deleting)
                        {
                                if(fclose(tmp))
                                {
                                        perror("Error : Couldn't save changes to temporary file ");
                                        exit(FCLOSE_ERR);
                                }
                                fclose(fp);
                                remove(fname);
                                if(rename(TMP_FNAME,fname))
                                {
                                        perror("Error : Couldn't save changes to file ");
                                        exit(RENAME_ERR);
                                }
                        }
                        break;
                }
                case 'r' : case 'R' :
                {
                        if(argc < 3)
                        {
                                fputs("Not enough arguments.\nFormat for \"r\" option :\n\tmclipm r <file>\n",stderr);
                                exit(NOT_ENOUGH_ARGS);
                        }
                        FILE * fp = fopen(argv[2],"r");
                        if(!fp)
                        {
                                perror("Error : Couldn't open file to read ");
                                exit(FOPEN_ERR);
                        }
                        for(int c; (c = fgetc(fp)) != EOF; putchar(c)) ;
                        break;
                }
                default :
                fputs("Error : Unrecognised arguments.\n",stderr);
                exit(INAPPROPIATE_ARGS);
        }
        exit(EXIT_SUCCESS);
}
