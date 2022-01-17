#ifdef _WIN32
	#define WIN
	#define _CRT_RAND_S /* Required before #include's for rand_s() */
	#define _CRT_SECURE_NO_WARNINGS
	#define _WIN32_WINNT _WIN32_WINNT_WINXP
#elif defined __unix__ || (defined __APPLE__ && defined __MACH__)
	#define NIX
	#define NIX_RAND_DEV "/dev/urandom"
#else
	#define OTHER
#endif

#include "mclipm.h"
#include <inttypes.h> /* uintmax_t, strtoumax      */
#include <ctype.h>    /* tolower                   */
#include <time.h>     /* time_t, time, ctime       */

char *rm_newline(char *s, size_t len)
{
	if(len >= 2 && s[len-2] == '\r') 
		s[len-2] = '\0';
        else if(len >= 1 && (s[len-1] == '\n' || s[len-1] == '\r'))
		s[len-1] = '\0';
	return s;
}

const char *time_str(void)
{
	char *s = ctime( &(time_t){time(NULL)} );
	return s? rm_newline(s, 25) : "";
}

size_t chkd_size_t_add(const size_t vals[])
{
	size_t sum = 0;
	for(size_t i = 0; vals[i] != 0; i++) {
		if(SIZE_MAX-sum >= vals[i])
			sum += vals[i];
		else
			return OVRFLW;
	}
	return sum;
}

bool iscasesubstr(const char *restrict s, const char *restrict sub)
{
	if(*sub == '\0')
		return true;
	for(; *s != '\0'; s++) {
		if(tolower(*s) == tolower(*sub)) {
			size_t i;
			for(i = 1; s[i] != '\0' && sub[i] != '\0'
					&& tolower(s[i]) == tolower(sub[i]); i++)
				;
			if(sub[i] == '\0')
				return true;
			else
				s += i;
		}
	}
	return false;
}

char *csv_fmt(const char *raw)
{
	size_t len = 0, quotes = 0;
	for(; raw[len] != '\0'; len++) {
		if(raw[len] == '"')
			quotes++;
	}	
	/* CHKDADD(...) will stop evaluating at a 0 arg !
	 * Thus, pass the constants first, as they must not be ignored.
	 * Then, the len is likely to be non-0 even if quotes == 0. So pass it prior to the latter.
	 */
	size_t sz = CHKDADD(1, 2, len, quotes); /* 1 for '\0', 2 for "" */
	if(sz == OVRFLW)
		return NULL;
	char *res = malloc(sz);
	if(res) {
		/* 1. Add quotes to begining & end. 
         	 * 2. Escape every '"' with another.
		 * 3. Copy other chars as-is. Terminate with '\0'.
         	 */
		size_t j = 0;
		res[j++] = '"';
		for(size_t i = 0; raw[i] != '\0'; res[j++] = raw[i++]) {
			if(raw[i] == '"')
				res[j++] = '"';
		}
		res[j++] = '"', res[j] = '\0';
	}
	return res;
}

char *mkpwd(size_t len, char upper_lim, char lower_lim)
{
	char *pwd;
	if(SIZE_MAX-1 < len || (pwd = malloc(len+1)) == NULL)
		return NULL;
	#ifdef NIX
		FILE *randf = fopen(NIX_RAND_DEV, "rb");
		if(randf == NULL) {
			free(pwd);
			return NULL;
		}
	#elif defined OTHER
		srand(time(NULL));
	#endif		
	
	for(pwd[len] = '\0'; len --> 0;) {
		#if defined WIN
			unsigned random;
			errno_t error = rand_s(&random);
			if(error) {
				free(pwd);
				return NULL;
			}
		#elif defined NIX
			int random = fgetc(randf);
			if(random == EOF) {
				free(pwd);
				return NULL;
			}	
		#else
			int random = rand();
		#endif
		pwd[len] = (random % (upper_lim - lower_lim + 1)) + lower_lim;
	}
	#ifdef NIX
		fclose(randf);
	#endif
	return pwd;
}

bool strtosize_t(size_t *dst, const char *restrict src)
{
	if(*src == '-' || strempty(src))
		return false;

	char *end = NULL;
	uintmax_t tmp = strtoumax(src, &end, 10);
	
	if(errno == ERANGE || tmp > SIZE_MAX || !strempty(end) || tmp == 0)
		return false;
	else {
		*dst = tmp;
		return true;
	}
}

MGA_DEF(dystr)

enum {LNSZ = 256}; /* Growth factor for getln() */
/* Reads line from src to dst->arr (retains EOL),
 * sets dst->len to the '\0' byte.
 *
 * Returns false & dst->len = 0 on alloc error,
 * ferror(src) or feof(src), leaving
 * dst->arr indeterminate.
 */
bool getln(struct dystr *dst, FILE *src)
{
	dst->len = 0;
	do {
		/* Check feof() first to avoid unnecessary allocation */
		if(feof(src) || !dystr_reserve(dst, dst->len+LNSZ+1)
				|| fgets(dst->arr+dst->len, dst->cap-dst->len, src) == NULL) {
			return false;
		} else
			dst->len += strlen(dst->arr + dst->len);
	} while(dst->arr[dst->len-1] != '\n');
	return true;
}
