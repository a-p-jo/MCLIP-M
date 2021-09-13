#include "mclipm.h"

char *rm_newline(char *str, const size_t len)
{
	if(len >= 2 && str[len-2] == '\r') 
		str[len-2] = '\0';
        else if(len >= 1 && (str[len-1] == '\n' || str[len-1] == '\r')) 
		str[len-1] = '\0';

	return str;
}

char *time_str(void)
{
	time_t timenum  = time(0);
	char *time_str  = ctime(&timenum);
        
        if(time_str)
		return rm_newline(time_str,25);
        else
		return NULL;
}

size_t chkd_size_t_add(const size_t *vals)
{
	size_t sum;
	for(sum = 0; *vals; vals++) {
		size_t val = *vals;
		if(SIZE_MAX - sum >= val)
			sum += val;
		else
			return 0;
	}
	return sum;
}

size_t chkd_size_t_mul(const size_t *vals)
{
	size_t prod;
	for(prod = 1; *vals; vals++) {
		size_t val = *vals;
		size_t result = prod * val;
		if(result/prod == val)
			prod = result;
		else
			return 0;

	}
	return prod;
}


char *csv_fmt(const char *raw)
{
        /* Determine strlen and number of quotes to be added */
        size_t len, quotes;
        for(len = 0, quotes = 0; raw[len]; len++) {
                if(raw[len] == '"')
			quotes++;
	}

	/* Check for overflow  & malloc net size needed. */

	/* Why in that order ?
	 * CHKDADD(...) will stop evaluating at a 0 arg !
	 * Thus, pass the constants first, as they must not be ignored.
	 * Then, the len is likely to be non-0 even if quotes == 0. So pass it prior to the latter.
	 */
	size_t bytes = CHKDADD(1,2,len,quotes);
	if(!bytes)
		return NULL;

	char *rval = malloc(bytes);

	if(rval) {
		/* 1. Add quotes to beginning & end. 
         	 * 2. Escape every double quote in raw str with another.
		 * 3. Copy all other chars as-is. Terminate with '\0'.
         	 */
		rval[0] = '"';
		size_t q = 1;
		for(size_t i = 0; raw[i]; rval[q++] = raw[i++]) {
			if(raw[i] == '"')
				rval[q++] = '"';
		}
		rval[q] = '"'; 
		rval[q+1] = '\0';
		return rval;
	} else
		return NULL;
}

static char *dystr_mgr(struct dystr *dystr) 
{
	/* If str is NULL (uninitialized) allocate to initial size + 1 (for '\0').
	 * Thereafter, realloc by multiple of growth factor (the extra 1 byte stays). 
	 * Check for overflows.
	 */
	size_t newsz;
	if(dystr->str) {
		if( !( newsz = CHKDMUL(DYSTR_GROWBY,dystr->sz) ) || !( newsz = CHKDADD(1,newsz) ) ) 
		error :
		{ 
				free(dystr->str);
				return NULL;
		}
	} else
		newsz = DYSTR_INITSZ + 1;

	void *tmp = realloc(dystr->str,newsz);
	if(tmp) {
		dystr->str = tmp;
		dystr->sz  = newsz;
		return dystr->str;
	} else
		goto error;
}

char *strlower(struct dystr *restrict dest, const char *restrict src)
{
	size_t i;
	for(i = 0; src[i]; i++) {
		if(i == dest->sz && !dystr_mgr(dest))
			return NULL;
		dest->str[i] = tolower(src[i]);
	}
	dest->str[i] = '\0';
	dest->len    = i;
	return dest->str;
}


char *mkpswd(const size_t len,const char upper_lim,const char lower_lim)
{
	size_t bytes = CHKDADD(1,len);
	if(!bytes)
		return NULL;

	char *pswd = malloc(bytes);	
	if(!pswd) 
		return NULL;

	#if defined NIX
		FILE *randf = fopen(NIX_RAND_DEV,"rb");
		if(!randf)
			return NULL;
	#elif defined OTHER
		srand(time(0));
	#endif		
	
	for(size_t i = 0; i < len; i++) {
		#if defined WIN
			unsigned random;
			/* "errno_t rand_s(unsigned int *)" - Microsoft docs */
			errno_t error = rand_s(&random);
			if(error)
				return NULL;
		#elif defined NIX
			int random = fgetc(randf);
			if(random == EOF)
				return NULL;
		#else
			int random = rand();
		#endif

		/* Force random number into range */
		pswd[i] = (random % (upper_lim - lower_lim + 1)) + lower_lim;
			
	}
	pswd[len] = '\0';
	#ifdef NIX
		fclose(randf);
	#endif
	return pswd;
}

char *strtosize_t(const char *restrict str, size_t *restrict result)
{
	/* wraparound in strtoumax, explicit check needed */
	if(*str == '-')
		return "value < 1";

	char *end = NULL; 
	errno = 0;
	uintmax_t tmp = strtoumax(str,&end,10);
	
	if(errno == ERANGE || tmp > SIZE_MAX)
		return "value too big";
	else if(*end)
		return "non-digits invalid";
	else if(!tmp)
		return "value  < 1";
	else {
		*result = tmp;
		return NULL;
	}
}

char *getln(FILE *restrict fp, struct dystr *restrict buf)
{
	for(size_t i = 0;;) {
		if(i == buf->sz && !dystr_mgr(buf))
			return NULL;

		int ch = getc(fp);

		if(ch == EOF) {
			if(feof(fp)) {
				buf->str[i] = '\0';
				buf->len = i;
				return buf->str;
			} else 
			error :
			{
				free(buf->str);
				return NULL;
			}
		}
		
		buf->str[i] = ch;
		
		if(ch == '\n') {
			/* There's always space for '\0' */
			buf->str[i+1] = '\0';
			buf->len = i+1;
			return buf->str;
		}

		if( !(i = CHKDADD(1,i)) )
			goto error;
	}
}
