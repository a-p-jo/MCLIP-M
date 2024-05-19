#ifdef _WIN32
	#define OS_WIN
	#define _CRT_RAND_S /* Required before #include's for rand_s() */
#elif defined __unix__ || (defined __APPLE__ && defined __MACH__)
	#define OS_NIX
	#define NIX_RAND_DEV "/dev/urandom"
#else
	#define OS_OTHER
#endif

#include "mclipm.h"
#include <inttypes.h> /* uintmax_t, strtoumax      */
#include <time.h>     /* time_t, time, ctime       */

const char *time_str(void)
{
	char *s = ctime( &(time_t){time(NULL)} );
	return s? rm_newline(s, 25) : "";
}

static inline char clwr(char c) { return c >= 'A' && c <= 'Z' ? c+32 : c; }

bool iscasesubstr(const char *restrict s, const char *restrict sub)
{
	if (strempty(sub))
		return true;
	for (; *s; s++) {
		if (clwr(*s) == clwr(*sub)) {
			size_t i;
			for (i = 1; s[i] && sub[i] && clwr(s[i]) == clwr(sub[i]); i++)
				;
			if (sub[i] == '\0')
				return true;
			else
				s += i;
		}
	}
	return false;
}

char *csvfmt(const char *raw)
{
	size_t len = 0, quotes = 0;
	for(; raw[len]; len++) {
		if(raw[len] == '"')
			quotes++;
	}
	size_t sz = UCHKDSUM(size_t, len, quotes, 2, 1);
	char *res = NULL;
	if (sz && (res = malloc(sz))) {
		/* 1. Add quotes to begining & end. 
         	 * 2. Escape every '"' with another.
		 * 3. Copy other chars as-is. Terminate with '\0'.
         	 */
		size_t j = 0;
		res[j++] = '"';
		for(size_t i = 0; raw[i]; res[j++] = raw[i++]) {
			if(raw[i] == '"')
				res[j++] = '"';
		}
		res[j++] = '"', res[j] = '\0';
	}
	return res;
}

char *mkpwd(size_t len, char minch, char maxch)
{
	bool ovrflw = !UCHKDSUM(size_t, len, 1);
	char *pwd;
	if (ovrflw || !(pwd = malloc(len+1)))
		return NULL;

	#ifdef OS_NIX
	FILE *randf = fopen(NIX_RAND_DEV, "rb");
	if (!randf) {
		free(pwd);
		return NULL;
	}
	#elif defined OS_OTHER
	srand(time(NULL));
	#endif		
	
	for (pwd[len] = '\0'; len --> 0;) {
		#if defined OS_WIN
		unsigned random;
		errno_t err = rand_s(&random);
		if (err) {
			free(pwd);
			return NULL;
		}
		#elif defined OS_NIX
		int random = fgetc(randf);
		if (random == EOF) {
			free(pwd);
			return NULL;
		}	
		#else
		int random = rand();
		#endif
		pwd[len] = (random % (maxch - minch + 1)) + minch;
	}
	return pwd;
}

bool strtosize_t(size_t *dst, const char *restrict src)
{
	if(*src == '-' || strempty(src))
		return false;
	char *end = NULL;
	int olderrno = errno; errno = 0;
	uintmax_t tmp = strtoumax(src, &end, 10);
	int newerrno = errno; errno = olderrno;
	if(newerrno == ERANGE || tmp > SIZE_MAX || !strempty(end) || tmp == 0)
		return false;
	else {
		*dst = tmp;
		return true;
	}
}

SBOMGA_DEF(extern, dystr, realloc, free)

/* Reads line from src to dst->arr (retains EOL),
 * sets dst->len to the '\0' byte.
 *
 * Returns false on alloc error,
 * ferror(src) or feof(src), leaving
 * dst->arr indeterminate.
 */
bool getln(dystr *dst, FILE *src)
{
	do {
		/* Check feof() first to avoid unnecessary allocation */
		if (!dystr_reserve(dst, dst->len+1+1)
		|| !fgets(dystr_arr(dst)+dst->len, dystr_cap(dst)-dst->len, src))
			return false;
		else
			dst->len += strlen(dystr_arr(dst)+dst->len);
	} while (dystr_arr(dst)[dst->len-1] != '\n' && !feof(src));
	return true;
}
