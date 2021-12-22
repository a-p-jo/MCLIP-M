#if !defined __STDC_HOSTED__ || __STDC_VERSION__ < 199901L
	#error "Hosted C99 implementation required !"
#endif

#include <stdbool.h> /* bool, true, false               */
#include <stddef.h>  /* size_t, NULL                    */
#include <string.h>  /* strcmp, strerror                */
#include <stdio.h>   /* fopen, fclose, FILE, fputs, puts,
                      * printf, fprintf, fgetc          */
#include <errno.h>   /* errno, ERANGE                   */
#include <stdlib.h>  /* malloc, free, srand, rand, exit,
			EXIT_FAILURE, EXIT_SUCCESS      */
#include "mga.h"     /* MGA_DECL, MGA_DEF               */

#define NAME          "MCLIPM v3.2"
#define TMPF          ".mclipm.tmp"
#define DEFAULT_PFILE "mclipm.csv"
enum { 
	DEFAULT_PWD_LEN     = 32 ,
	DEFAULT_RANGE_UPPER = '~',
	DEFAULT_RANGE_LOWER = '!', /* No whitespace by default */
	DYSTR_INITCAP       = 100  /* Initial char capacity of dystr buffers */
};

#define ERROR_EXIT(msg) do { \
	fprintf(stderr, "Error : "msg"%s%s\n", errno? " : " : ".", errno? strerror(errno) : ""); \
	exit(EXIT_FAILURE); \
} while(0)

static bool streq(const char *s1, const char *s2) { return strcmp(s1, s2) == 0;}
static bool strempty(const char *s) { return *s == '\0'; }

/* Removes CR/LF/CRLF from str[len-1] in-place */
char *rm_newline(char *s, size_t len);

/* Returns ctime() string, stripped of '\n' */
char *time_str(void);

/* Returns 0 if sum overflows, else sum of vals */
enum {OVRFLW};
size_t chkd_size_t_add(const size_t vals[]);
#define CHKDADD(...) chkd_size_t_add((const size_t []) {__VA_ARGS__,0})

char *strcasestr(const char *str, const char *substr);

/* Returns malloc'd csv-formatted copy of raw or NULL */ 
char *csv_fmt(const char *raw);

/* Uses OS API/rand() to make & return malloc'd string of len random chars.
 * upper_lim >= char >= lower_lim
 */
char *mkpwd(size_t len, char upper_lim, char lower_lim);

/* Converts src to a size_t value and stores at *dst on success. */
bool strtosize_t(size_t *dst, const char *restrict src);

MGA_DECL(dystr, char)

/* Returns line from src copied to alloced dst->arr. 
 * If error, dystr_destroy()s dst & returns NULL.
 */
char *getln(struct dystr *dst, FILE *src);

struct opts {
	size_t pwdlen;
	char *restrict outfile, *restrict comments;
	struct {
		char upper;
		char lower; 
	} range;
};

/* For the "g" command, parse argv for valid options, 
 * evaluate if any and return struct opts with corresponding values.
 * Bad options result in error messages and termination.
 */
struct opts getopts(char **argv);

/* Call csv_fmt on pswd & comments and write them comma-seperated to dst */
void mkentry(FILE *dst, const char *restrict pswd, const char *restrict comments);

/* Case-insensitively strstr's given terms for 1st match in lines of file.
 * Prints line with a match. If deleting, deletes line with a match. 
 */
void find_del(const char *restrict fname, bool deleting, char **terms);
