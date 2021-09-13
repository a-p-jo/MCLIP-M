/* ---------------------------
 * Preprocessor system checks |
 * ---------------------------
 */

/* Reject compilation on freestanding or older than C99 compiler */
#if !defined __STDC_HOSTED__
	#error "Hosted C99 implementation required !"
#elif __STDC_VERSION__ < 199901L
	#error "C99 or later required !"
#endif
	
/* Identify OS/API for PRNG used in password generation */
#if defined _WIN32
	/* Tested on Windows 7 with msvc, clang-cl and mingw-w64 */
	#define _CRT_SECURE_NO_WARNINGS         /* Shut up, MSVC */
	#define _WIN32_WINNT _WIN32_WINNT_WINXP /* Support XP and newer */
	#define _CRT_RAND_S                     /* Required before #includes, see MS docs */
	#define WIN

#elif defined __unix__ || (defined __APPLE__ && defined __MACH__)
	/* Tested on Linux 5.10 with glibc and musl, clang and gcc
	 * Tested on macOS 11.5.2 with tcc and clang.
	 */
	#define NIX_RAND_DEV "/dev/urandom"     /* Change if you need */
	#define NIX
#else
	#define OTHER
#endif

/* ------------------------------------------------
 * Dependencies on the hosted C99 standard library |
 * ------------------------------------------------
 */

#include <stdio.h>    /* fopen, fclose, FILE *, fputs, printf, strerror, fprintf, getc */
#include <stdlib.h>   /* malloc, realloc, free, exit, EXIT_SUCCESS, EXIT_FAILURE, rand */
#include <errno.h>    /* errno, ERANGE                                                 */
#include <time.h>     /* time, ctime, time_t                                           */
#include <string.h>   /* strstr, strlen, strcmp                                        */
#include <ctype.h>    /* tolower                                                       */
#include <stdbool.h>  /* bool, true, false                                             */
#include <stdint.h>   /* SIZE_MAX, uintmax_t                                           */
#include <inttypes.h> /* strtoumax                                                     */

/* ----------------
 * Macro Constants |
 * ----------------
 */

#define NAME                "MCLIPM v3"

/* Defaults */
#define DEFAULT_PWD_LEN     32
#define TMP_FNAME           ".temp.mclipm"
#define DEFAULT_PWD_FILE    "mclipm.csv" /* When filename not specified, paswords saved here    */
#define DEFAULT_RANGE_UPPER '~'
#define DEFAULT_RANGE_LOWER '!'          /* Disallows whitespace in generated password          */ 
#define DYSTR_INITSZ        1024         /* Initial malloc'd size of dynamic string             */
#define DYSTR_GROWBY        2            /* When full, dynamic string realloc'd by its multiple */

/* ---------------------------------------------------------------
 * Functions that transform data and return it.                   | 
 * These never print, cause program termination or touch globals. |
 * ---------------------------------------------------------------
 */

/* Remove CR/LF/CRLF from '\0'-terminated str of given length in-place */
char *rm_newline(char *str, const size_t len);

/* Return ctime() string, stripped of '\n' */
char *time_str(void);

/* Checks sum of 0-terminated array of size_t values for overflow, returns 0 if so */
size_t chkd_size_t_add(const size_t *vals);

/* Syntactic sugar for chkd_size_t_add().
 *
 * IMPORTANT : the 'chkd' functions assume a 0-terminated array. 
 * If any argument evaluates to 0, processing terminates right there ! 
 */
#define CHKDADD(...) chkd_size_t_add((size_t []) {__VA_ARGS__,0})

/* Checks product of 0-terminated array of size_t values for overflow, returns 0 if so */
size_t chkd_size_t_mul(const size_t *vals);

/* Syntactic sugar for chkd_size_t_mul()
 *
 * IMPORTANT : the 'chkd' functions assume a 0-terminated array. 
 * If any argument evaluates to 0, processing terminates right there ! 
 */
#define CHKDMUL(...) chkd_size_t_mul((size_t []) {__VA_ARGS__,0})

/* Return malloc'd copy of raw str with quotes csv-formmated, or NULL */ 
char *csv_fmt(const char *raw);

/* Dynamic string used in strlower & getln */
struct dystr {
	char *str;  /* '\0'-terminated buffer                             */
	size_t sz;  /* bytes alloced for str (not including one for '\0') */
	size_t len; /* bytes in use in str                                */
};

/* Returns alloced copy of src with all chars tolower()'d.
 * Allocates dest->str if NULL, grows it if too small; updates dest->len.
 * If error, frees dest->str and returns NULL.
 */
char *strlower(struct dystr *restrict dest, const char *restrict src);

/* Use OS API/rand() to make malloc'd, '\0'-terminated string of len random chars and
 * upper_lim >= char >= lower_lim
 */
char *mkpswd(const size_t len, const char upper_lim, const char lower_lim);

/* Convert str to a size_t value and store at *result.
 * Return error message or NULL on success. Result only assigned to on success.
 */
char *strtosize_t(const char *restrict str, size_t *restrict result);

/* Return line from fp copied to alloced buf->str. 
 * Mallocs buf->str if NULL, grows it if too small; updates buf->len. 
 * If error, frees buf->str & returns NULL.
 */
char *getln(FILE *restrict fp, struct dystr *restrict buf);

/* ---------------------------------------------------------------------
 * Functions that do an action.                                         |
 * These print messages, may cause program termination and use globals. |
 * ---------------------------------------------------------------------
 */

/* Data returned by getopts stored this way */
struct opts {
	size_t pswd_len;
	char *save_to;   /* output filename       */
	char *comments;  /* comments for password */
	struct {
		char upper;
		char lower; 
	} range;        /* ASCII limits for password's chars */
};

/* For the "g" command, parse argv for valid options, 
 * evaluate if any and return struct opts with corresponding values.
 * Bad options result in error messages and termination.
 */
struct opts getopts(char **argv);

/* Call csv_fmt on pswd & comments and write them comma-seperated to fp and flush */
void write_entry(FILE *restrict fp, char *restrict pswd, char *restrict comments);

/* Case-insensitively strstr's given terms for 1st match in lines of file.
 * Prints line with a match. If deleting, deletes line with a match. 
 */
void find_del(const char *restrict fname, const bool deleting, char **terms);
