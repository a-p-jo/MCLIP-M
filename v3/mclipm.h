#include <stdbool.h>
#include <stddef.h>  /* size_t, NULL                    */
#include <stdint.h>  /* uintmax_t 		        */
#include <string.h>  /* strcmp, strerror                */
#include <stdio.h>   /* fopen, fclose, FILE, fputs, puts,
                      * printf, fprintf, fgetc          */
#include <errno.h>   /* errno, ERANGE                   */
#include <stdlib.h>  /* malloc, free, srand, rand, exit,
			EXIT_FAILURE, EXIT_SUCCESS      */
#include "sbomga.h"

#define MCLIPM_VERSION               "3.5"
#define DEFAULT_TEMPORARY_FILE_NAME  ".mclipm.tmp"
#define DEFAULT_PWD_FILE_NAME        "mclipm.csv"
#define DEFAULT_PWD_LEN              16
#define DEFAULT_PWD_MINCHAR          '!'
#define DEFAULT_PWD_MAXCHAR          '~' /* No whitespace */

#define ERROR_EXIT(msg) do {                                           \
	fputs("Error : "msg"\n", stderr);                              \
	int e = errno;                                                 \
	if (e) fprintf(stderr, "System Message : %s.\n", strerror(e)); \
	exit(EXIT_FAILURE);                                            \
} while (0)

static inline bool streq(const char *s1, const char *s2) { return strcmp(s1, s2) == 0;}
static inline bool strempty(const char *s) { return *s == '\0'; }

/* Removes CR/LF/CRLF from str[len-1] in-place */
static inline char *rm_newline(char *restrict s, size_t len)
{
	if (len >= 2 && s[len-2] == '\r') 
		s[len-2] = '\0';
	else if (len >= 1 && (s[len-1] == '\n' || s[len-1] == '\r'))
		s[len-1] = '\0';
	return s;
}

/* Returns ctime() string, stripped of '\n' */
const char *time_str(void);

/* Returns 0 if sum overflows, else sum of n vals */
static inline uintmax_t uchkdsum(const uintmax_t *vals, uint_least8_t n, uintmax_t maxval)
{
	uintmax_t sum = 0;
	while (n --> 0) {
		if (maxval-sum >= vals[n])
			sum += vals[n];
		else
			return 0;
	}
	return sum;
}
#define UCHKDSUM(T, ...)                                        \
uchkdsum(                                                       \
	(uintmax_t[]){__VA_ARGS__},                             \
	sizeof((uintmax_t[]){__VA_ARGS__}) / sizeof(uintmax_t), \
	(T)-1                                                   \
)

bool iscasesubstr(const char *restrict str, const char *restrict sub);

/* Returns malloc'd csv-formatted copy of raw or NULL */ 
char *csvfmt(const char *raw);

/* Returns malloc'd string of len random chars such each is in [min, max]  or NULL. */
char *mkpwd(size_t len, char minch, char maxch);

/* Converts src to a size_t value and stores at *dst on success. */
bool strtosize_t(size_t *dst, const char *restrict src);

SBOMGA_DECL(extern, dystr, 0, char)

/* Reads line from src to dst->arr (retains EOL),
 * sets dst->len to the '\0' byte.
 *
 * Returns false & dst->len = 0 on alloc error,
 * ferror(src) or feof(src), leaving
 * dst->arr indeterminate.
 */
bool getln(dystr *dst, FILE *restrict src);

struct opts {
	size_t pwdlen;
	const char *restrict outfile, *restrict comments;
	struct { char max, min; } chrange;
};

/* For the "g" command, parse argv for valid options, 
 * evaluate if any and return struct opts with corresponding values.
 * Bad options result in error messages and termination.
 */
struct opts getopts(const char *const restrict *restrict argv);

/* Call csvfmt on pswd & comments and write them comma-seperated to dst */
void mkentry(FILE *restrict dst, const char *restrict pswd, const char *restrict comments);

/* Case-insensitively strstr's given terms for 1st match in lines of file.
 * Prints line with a match. If deleting, deletes line with a match. 
 */
void find_del(const char *restrict fname, bool deleting, const char *const restrict *restrict terms);
